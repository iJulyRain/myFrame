/*
 * =====================================================================================
 *
 *       Filename:  ssocks.c
 *
 *    Description:  ssocks
 *
 *        Version:  1.0
 *        Created:  2016年05月28日 23时15分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (g3), lizhixian@integritytech.com.cn
 *   Organization:  g3
 *
 * =====================================================================================
 */

#include "common.h"
#include "ssocks.h"

#define NAME "ssocks"
#include <netdb.h>

static int thread_proc(HMOD hmod, int message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case MSG_INIT:
		{
			char settings[32];
			debug(DEBUG, "### '%s'\tMSG_INIT\n", NAME);

			object_io_t server; //listen proxychain

			///<创建一个server, 监听proxychains
			server = new_object_io_tcp_server("ssocks server");
			assert(server);

			memset(settings, 0, sizeof(settings));
			sprintf(settings, "0.0.0.0:%d:20", global_conf.listen_port);

			server->_info();
			server->_init(&server->parent, hmod, settings);
		}
			break;

		case MSG_AIOCONN:
		{
			object_io_t io, bio;
			io = (object_io_t)lparam;

			if (io->mode == mode_tcp_server)
				break;

			else if (io->mode == mode_tcp_server_client)
			{
				struct cb *cb = (struct cb *)calloc(1, sizeof(struct cb));
				assert(cb);

				cb->state = socks_state_version;
				io->user_ptr = cb; 
			}
			else if (io->mode == mode_tcp_client)
			{
				bio = (object_io_t)io->user_ptr;
				struct cb *cb = (struct cb *)bio->user_ptr;

				if (cb->state == socks_state_connect)
				{
					char buffer[16];
					memset(buffer, 0, sizeof(buffer));

					if (io->isconnect == ONLINE)
					{
						buffer[0] = 0x05;
						buffer[1] = 0x00;
						buffer[2] = 0x00;
						buffer[3] = 0x01;
					}
					else if(io->isconnect == OFFLINE)
					{
						buffer[0] = 0x05;
						buffer[1] = 0x01;
						buffer[2] = 0x00;
						buffer[3] = 0x01;
					}

					buffer[4] = 0x00; //匿名IP
					buffer[5] = 0x00;
					buffer[6] = 0x00;
					buffer[7] = 0x00;

					buffer[8] = 0x00; //匿名端口
					buffer[9] = 0x00;

					bio->_output(&bio->parent, buffer, 10);
					cb->state = socks_state_stream;
				}
			}
		}
			break;

		case MSG_AIOIN:
		{
			int rxnum;

			object_io_t io = (object_io_t)lparam;
			object_io_t bio;

			if (io->mode == mode_tcp_server)
				break;
			else if (io->mode == mode_tcp_client)
			{
				char buffer[BUFFER_MAX];

				memset(buffer, 0, sizeof(buffer));

				bio = (object_io_t)io->user_ptr;

				rxnum = io->_input(&io->parent, buffer, BUFFER_MAX, TRUE);
				if (bio)
					bio->_output(&bio->parent, buffer, rxnum);
			}
			else if (io->mode == mode_tcp_server_client)
			{
				struct cb *cb = (struct cb *)io->user_ptr;

				switch (cb->state)
				{
					case socks_state_version:
					{
						char buffer[8];

						memset(buffer, 0, sizeof(buffer));
						rxnum = io->_input(&io->parent, buffer, 8, TRUE);
						if(rxnum < 3)
						{
							debug(RELEASE, "==> invalid version request!\n");
							io->_close(&io->parent);

							break;
						}

						if (buffer[0] != 0x05) //不是socks5代理
						{
							debug(RELEASE, "==> detect ver[%d], but we only Support SOCKS5!\n", buffer[0]);
							io->_close(&io->parent);
							break;
						}

						char method = -1;
						int i;
						for (i = 0; i < buffer[1] && i < 4; i++)
						{
							method = buffer[2 + i]; 
							if(method == 0)//目前先支持匿名方式
								break;
						}

						if (method != 0)
						{
							debug(RELEASE, "==> method error: %d!\n", method);
							io->_close(&io->parent);
							break;
						}
						debug(DEBUG, "==> SOCKS5 client verify!\n");

						memset(buffer, 0, sizeof(buffer));
						buffer[0] = 0x05;
						buffer[1] = 0x00;
						
						io->_output(&io->parent, buffer, 2);	//ver response

						cb->state = socks_state_connect;
					}
						break;
					case socks_state_connect:
					{
						char buffer[16];
						object_io_t cio;

						memset(buffer, 0, sizeof(buffer));
						rxnum = io->_input(&io->parent, buffer, 16, TRUE);
						if (rxnum < 10)
						{
							debug(RELEASE, "==> invalid connection request!\n");
							io->_close(&io->parent);
							break;
						}

						if (buffer[0] != 0x05)
						{
							debug(RELEASE, "==> detect ver[%d], but we only Support SOCKS5!\n", buffer[0]);
							io->_close(&io->parent);
							break;
						}

						if (buffer[1] != 0x01)
						{
							debug(RELEASE, "==> detect CMD[%d], but we only Support CONNECT(0x01)!\n", buffer[1]);
							io->_close(&io->parent);
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
								io->_close(&io->parent);
								io->isconnect = REMOVE;
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

						///<create tcp client
						char settings[32];
						memset(settings, 0, sizeof(settings));
						sprintf(settings, "%s:%s", ip, port);

						cio = new_object_io_tcp(settings);
						assert(cio);

						cio->_info();
						cio->_init(&cio->parent, hmod, settings);
						cio->user_ptr = io;

						cb->bind = cio;
					}
						break;
					case socks_state_stream:
					{
						char buffer[BUFFER_MAX];

						memset(buffer, 0, sizeof(buffer));

						rxnum = io->_input(&io->parent, buffer, BUFFER_MAX, TRUE);
						if (cb->bind)
							cb->bind->_output(&cb->bind->parent, buffer, rxnum);
					}
						break;
				}
			}
		}
			break;
		case MSG_AIOERR:
		case MSG_AIOBREAK:
		{
			object_io_t io = (object_io_t)lparam;
			if (io->mode == mode_tcp_client)
			{
				object_io_t bio = (object_io_t)io->user_ptr;
				bio->_close(&bio->parent);
			}
			else if(io->mode == mode_tcp_server_client)
			{
				struct cb *cb = (struct cb *)io->user_ptr;
				cb->bind->_close(&cb->bind->parent);
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
int register_thread_ssocks(void)
{
    object_thread_t ot;

    ot = new_object_thread(thread_proc); 
	assert(ot);

    object_addend(&ot->parent, NAME, object_class_type_thread);

	return 0;
}
