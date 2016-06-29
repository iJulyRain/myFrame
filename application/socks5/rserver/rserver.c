/*
 * =====================================================================================
 *
 *       Filename:  rserver.c
 *
 *    Description:  rserver source
 *
 *        Version:  1.0
 *        Created:  2016年06月23日 15时03分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (group3), lizhixian@integritytech.com.cn
 *   Organization:  
 *
 * =====================================================================================
 */

#include "common.h"
#include "rserver.h"

#include <string.h>
#include <netdb.h>

#define NAME "R SERVER"

static object_io_t server_proxy; //listen proxychain
static object_io_t server_client; //listen client 

static object_io_t get_one_client(object_io_t server)
{
    object_io_t io = NULL;
    object_thread_t ot;
    struct control_block *cb;

    ot = (object_thread_t)server->hmod;

    CONTAINER_FOREACH(&ot->io_container, object_io_t, io)
        cb = (struct control_block *)io->user_ptr;

        if (io->mode == mode_tcp_server)
            continue;

        if(!strcmp(object_name(&io->server->parent), object_name(&server->parent))
        && io_state(&io->parent) == ONLINE
        && cb->io_bind == NULL)
            break;

        io = NULL;
    CONTAINER_FOREACH_END

    return io;
}

/**
* @brief loop process 
*
* @param hmod handler module
* @param message message
* @param wparam first argument
* @param lparam second argument
*
* @return success 0, failed -1 
*/
static int thread_proc(HMOD hmod, int message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case MSG_INIT:
		{
			char settings[32];

			debug(DEBUG, "### '%s'\tMSG_INIT\n", NAME);

			///<创建一个server, 监听proxychains
			server_proxy = new_object_io_tcp_server("server proxy");
			assert(server_proxy);

            ///< 启动server proxy
			memset(settings, 0, sizeof(settings));
			sprintf(settings, "0.0.0.0:%d:50:%d", global_conf.reverse_port, IO_POOL_MAX);

			server_proxy->_info();
			server_proxy->_init(&server_proxy->parent, hmod, settings);

            //////////////////////////////////////////////////////////////
			///<创建一个server, 监听client
			server_client = new_object_io_tcp_server("server client");
			assert(server_client);

            ///< 启动server client 
			memset(settings, 0, sizeof(settings));
			sprintf(settings, "0.0.0.0:%d:50:%d", global_conf.listen_port, IO_POOL_MAX);

			server_client->_info();
			server_client->_init(&server_client->parent, hmod, settings);
		}
			break;

		case MSG_AIOCONN:
		{
            object_io_t client, server, io_bind;
            struct control_block *cb;

            client = (object_io_t)lparam;

            if (client->mode == mode_tcp_server) ///<ignore
                break;

            server = client->server;
            debug(DEBUG, "==> client connect to <%s> [%d]\n", object_name(&server->parent), client->isconnect);

            if (!strcmp(object_name(&server->parent), "server proxy")) ///< from proxychain
            {
                io_bind = get_one_client(server_client);
                if (io_bind == NULL)
                {
				    debug(RELEASE, "==> Warnning: rclient maybe not running or clients use up!\n");
                    client->_close(&client->parent);
                    break;
                }

                cb = (struct control_block *)calloc(1, sizeof(struct control_block));
                assert(cb);

                cb->io_bind = io_bind;
                cb->state = socks_state_version;
                client->user_ptr = cb; 

                //////////////////////////////////////////////////////
                cb = (struct control_block *)io_bind->user_ptr;
                cb->state = socks_state_version;
                cb->io_bind = client;
            }
            else if (!strcmp(object_name(&server->parent), "server client")) ///< from client 
            {
                cb = (struct control_block *)calloc(1, sizeof(struct control_block));
                assert(cb);

                cb->io_bind = NULL;
                client->user_ptr = cb;
            }
		}
			break;
		case MSG_AIOIN:
		{
			object_io_t client, server;

            client = (object_io_t)lparam;
            server = client->server;

            if (client->mode == mode_tcp_server)
                break;

            if (!strcmp(object_name(&server->parent), "server proxy")) ///< from proxychain
            {
				int i, rxnum;
				char buffer[BUFFER_SIZE];
				struct control_block *cb;

				cb = (struct control_block *)client->user_ptr;
				if (cb->io_bind == NULL)
                {
                    client->_close(&client->parent);
					break;
                }

				switch (cb->state)
				{
					case socks_state_version:
					{
                        memset(buffer, 0, sizeof(buffer));
						rxnum = client->_input(&client->parent, buffer, BUFFER_SIZE, TRUE);
						if(rxnum < 3)
						{
							debug(RELEASE, "==> invalid version request!\n");
                            client->_close(&client->parent);
							break;
						}
						debug(DEBUG, "MSG_AIOIN: %d bytes!\n", rxnum);

						if (buffer[0] != 0x05) //不是socks5代理
						{
							debug(RELEASE, "==> detect ver[%d], but we only Support SOCKS5!\n", buffer[0]);
                            client->_close(&client->parent);
							break;
						}

						char method = -1;
						for (i = 0; i < buffer[1] && i < 4; i++)
						{
							method = buffer[2 + i]; 
							if(method == 0)//目前先支持匿名方式
								break;
						}

						if (method != 0)
						{
							debug(RELEASE, "==> method error: %d!\n", method);
                            client->_close(&client->parent);
							break;
						}

						debug(DEBUG, "SOCKS5 client verify!\n");

						memset(buffer, 0, BUFFER_SIZE);
						buffer[0] = 0x05;
						buffer[1] = 0x00;
						
						client->_output(&client->parent, buffer, 2);	//ver response

						cb->state = socks_state_connect;
                        ((struct control_block *)cb->io_bind->user_ptr)->state = socks_state_connect;
					}
						break;

					case socks_state_connect:
					{
						rxnum = client->_input(&client->parent, buffer, BUFFER_SIZE, TRUE);
						if (rxnum < 10)
						{
							debug(RELEASE, "==> invalid connection request!\n");
							client->_close(&client->parent);
							break;
						}

						if (buffer[0] != 0x05)
						{
							debug(RELEASE, "==> detect ver[%d], but we only Support SOCKS5!\n", buffer[0]);
							client->_close(&client->parent);
							break;
						}

						if (buffer[1] != 0x01)
						{
							debug(RELEASE, "==> detect CMD[%d], but we only Support CONNECT(0x01)!\n", buffer[1]);
							client->_close(&client->parent);
							break;
						}

						char ip[4];
						char port[2];

						if (buffer[3] == 0x03) //domain name
						{
							char domain[256];
							size_t len = 0;
							struct hostent *hp;

							memset(domain, 0, sizeof(domain));

							len = buffer[4];
							memcpy(domain, buffer + 5, len);

							debug(DEBUG, "==> domain: %s\n", domain);

							hp = gethostbyname(domain);
							if (hp == NULL)
							{
								debug(RELEASE, "==> Can't get '%s' from DNS!\n", domain);
								client->_close(&client->parent);
								break;
							}

							memcpy(ip, hp->h_addr, hp->h_length);
							memcpy(port, buffer + 5 + len, 2);
						}
						else if (buffer[3] == 0x01) //ip
						{
							memcpy(ip, buffer + 4, 4);
							memcpy(port, buffer + 8, 2);
						}

						//<包装协议，转发
						struct s_header s_header;

						memset(buffer, 0, BUFFER_SIZE);
						memset(&s_header, 0, sizeof(struct s_header));

						s_header.magic = 0x55AA;
						s_header.command = SOCK_CONNECT;

						memcpy(s_header.data + 0, ip, 4);
						memcpy(s_header.data + 4, port, 2);

						memcpy(buffer, &s_header, sizeof(struct s_header));

						///<transfer
						if (cb->io_bind != NULL)
							cb->io_bind->_output(&cb->io_bind->parent, buffer, sizeof(struct s_header));
					}
						break;

					case socks_state_stream:
					{
						rxnum = client->_input(&client->parent, buffer, BUFFER_SIZE, TRUE);
						if (cb->io_bind != NULL)
						    cb->io_bind->_output(&cb->io_bind->parent, buffer, rxnum);
					}
						break;
				}
            }
            else if (!strcmp(object_name(&server->parent), "server client")) ///< from rclient 
            {
            	int rxnum;
				char buffer[BUFFER_SIZE];
				struct s_header s_header;

				struct control_block *cb;

                memset(buffer, 0, sizeof(buffer));
				rxnum = client->_input(&client->parent, buffer, BUFFER_SIZE, TRUE);
				debug(DEBUG, "MSG_AIOIN: %d bytes!\n", rxnum);

                memset(&s_header, 0, sizeof(struct s_header));
                memcpy(&s_header, buffer, sizeof(struct s_header));
                if((s_header.magic & 0xFFFF) == 0x55AA)
                {
                    if ((s_header.command & 0xFF) == SOCK_HEART)
                    {
                        debug(DEBUG, "==> HEART BEART!\n");
                        break;
                    }
                }

				cb = (struct control_block *)client->user_ptr; 
				if (cb->io_bind == NULL)
					break;

				switch (cb->state)
				{
					case socks_state_connect: //<转成sock5协议
					{
						memset(&s_header, 0, sizeof(struct s_header));
						memcpy(&s_header, buffer, sizeof(struct s_header));

						if((s_header.magic & 0xFFFF) == 0x55AA 
						&& (s_header.command & 0x01) == SOCK_CONNECT
						&& (s_header.data[0] & 0xFF) == 0x00)
						{
							buffer[0] = 0x05;
							buffer[1] = 0x00;
							buffer[2] = 0x00;
							buffer[3] = 0x01;

							debug(DEBUG, "===> connect ok!\n");
						}
						else
						{
							buffer[0] = 0x05;
							buffer[1] = 0x01;
							buffer[2] = 0x00;
							buffer[3] = 0x01;

							debug(DEBUG, "===> connect bad!\n");
						}

						buffer[4] = 0x00; //匿名IP
						buffer[5] = 0x00;
						buffer[6] = 0x00;
						buffer[7] = 0x00;

						buffer[8] = 0x00; //匿名端口
						buffer[9] = 0x00;

                        if (cb->io_bind)
                        {
                            cb->io_bind->_output(&cb->io_bind->parent, buffer, 10);
                            ((struct control_block *)cb->io_bind->user_ptr)->state = socks_state_stream;
                        }

                        cb->state = socks_state_stream;
					}
						break;
					case socks_state_stream:
					{
						memset(&s_header, 0, sizeof(struct s_header));
						memcpy(&s_header, buffer, sizeof(struct s_header));

						if((s_header.magic & 0xFFFF) == 0x55AA)
                        {
                            if ((s_header.command & 0xFF) == SOCK_BREAK)
                            {
                                if (cb->io_bind)
                                    cb->io_bind->_close(&cb->io_bind->parent);
                            }
                        }
						else
						{
                            if (cb->io_bind)
							    cb->io_bind->_output(&cb->io_bind->parent, buffer, rxnum);
						}
					}
						break;
				}
            }
		}
			break;
		case MSG_AIOERR:
		case MSG_AIOBREAK:
		{
			struct control_block *cb;
			object_io_t client, server, io_bind;

            client = (object_io_t)lparam; 
            server = client->server;

			cb = (struct control_block *)client->user_ptr;
            if (cb == NULL || cb->io_bind == NULL)
                break;

            io_bind = cb->io_bind;
            cb->io_bind = NULL;

            cb = (struct control_block *)io_bind->user_ptr;
            if (cb != NULL)
                cb->io_bind = NULL;

            if (!strcmp(object_name(&server->parent), "server proxy")) ///< from proxychain 
            {
                ///<reset io
                debug(DEBUG, "==> RST_IO\n");
                struct s_header s_header;
                char buffer[128];

                memset(buffer, 0, sizeof(buffer));
                memset(&s_header, 0, sizeof(struct s_header));

                s_header.magic = 0x55AA;
                s_header.command = SOCK_BREAK;
                memcpy(buffer, &s_header, sizeof(struct s_header));

                io_bind->_output(&io_bind->parent, buffer, sizeof(struct s_header));
            }
            else if (!strcmp(object_name(&server->parent), "server client")) ///< from rclient 
            {
                ///<close proxychain connect
                io_bind->_close(&io_bind->parent);
            }
		}
			break;

        case MSG_AIOCLR:
        {
            object_io_t client;

            client = (object_io_t)lparam; 
            if (client->user_ptr)
            {
                free(client->user_ptr);
                client->user_ptr = NULL;
            }
        }
            break;
	}

	return thread_default_process(hmod, message, wparam, lparam);
}

/**
* @brief register thread module
*
* @return always 0 
*/
int register_thread_rserver(void)
{
    return create_thread(thread_proc, NAME, THREAD_USING_POLLER);
}
