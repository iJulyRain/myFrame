/*
 * =====================================================================================
 *
 *       Filename:  rcsocks_a.c
 *
 *    Description:  rcsocks a side 
 *
 *        Version:  1.0
 *        Created:  2016年04月02日 14时05分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (g3), lizhixian@integritytech.com.cn
 *   Organization:  g3
 *
 * =====================================================================================
 */

#include "common.h"
#include "rcsocks.h"

#include <string.h>
#include <netdb.h>

#define NAME "rcsocks A"

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
			debug(DEBUG, "### '%s'\tMSG_INIT\n", NAME);
			char settings[32];

			object_io_t server; //listen proxychain

			///<创建一个server, 监听proxychains
			server = new_object_io_tcp_server("rcsocks A side");
			assert(server);

			memset(settings, 0, sizeof(settings));
			sprintf(settings, "0.0.0.0:%d:20", global_conf.reverse_port);

			server->_info();
			server->_init(&server->parent, hmod, settings);
		}
			break;
		case MSG_AIOCONN:
		{
			object_io_t io = (object_io_t)lparam;
			object_io_t io_stream = NULL;
			object_thread_t ot = NULL;
			struct control_block *cb = NULL;

			debug(DEBUG, "MSG_AIOCONN: %s\n", object_name((object_t)io));

			if(!strcmp(object_name((object_t)io), "rcsocks A side")
			|| io->_state((object_t)io) == OFFLINE)	///<忽略
				break;

			///<从rcsocks_b 获取一个空闲连接 并绑定
			ot = (object_thread_t)object_find("rcsocks B", object_class_type_thread);
			assert(ot);

			send_message((HMOD)ot, MSG_COMMAND, GET_IO, (LPARAM)&io_stream);
			if (io_stream == NULL)
			{
				debug(RELEASE, "==> rssocks maybe not running!\n");
				io->_close(&io->parent);
				io->isconnect = REMOVE;

				break;
			}

			cb = (struct control_block *)calloc(1, sizeof(struct control_block));
			assert(cb);

			cb->bind_io = io_stream;
			cb->state = socks_state_version;

			io->user_ptr = cb;
			io_stream->user_ptr = io; //被绑定端记录绑定端数据
		}
			break;
		case MSG_AIOIN:
		{
			object_io_t io = (object_io_t)lparam;
			debug(DEBUG, "==> rcsocks A side MSG_AIOIN!\n");

			if(!strcmp(object_name((object_t)io), "rcsocks A side"))	///<表示有新连接
			{
				debug(DEBUG, "==> new client connect!\n");
			}
			else	///<有数据
			{
				int i, rxnum;
				char buffer[BUFFER_MAX];
				struct control_block *cb;

				cb = (struct control_block *)io->user_ptr;
				switch (cb->state)
				{
					case socks_state_version:
					{
						memset(buffer, 0, BUFFER_MAX);

						rxnum = io->_input(&io->parent, buffer, BUFFER_MAX, TRUE);
						if(rxnum < 3)
						{
							debug(RELEASE, "==> invalid version request!\n");
							io->_close(&io->parent);
							io->isconnect = REMOVE;

							break;
						}
						debug(DEBUG, "MSG_AIOIN: %d bytes!\n", rxnum);

						if (buffer[0] != 0x05) //不是socks5代理
						{
							debug(RELEASE, "==> detect ver[%d], but we only Support SOCKS5!\n", buffer[0]);
							io->_close(&io->parent);
							io->isconnect = REMOVE;
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
							io->_close(&io->parent);
							io->isconnect = REMOVE;
							break;
						}

						debug(DEBUG, "SOCKS5 client verify!\n");

						memset(buffer, 0, BUFFER_MAX);
						buffer[0] = 0x05;
						buffer[1] = 0x00;
						
						io->_output(&io->parent, buffer, 2);	//ver response

						cb->state = socks_state_connect;
					}
						break;

					case socks_state_connect:
					{
						rxnum = io->_input(&io->parent, buffer, BUFFER_MAX, TRUE);
						if (rxnum < 10)
						{
							debug(RELEASE, "==> invalid connection request!\n");
							io->_close(&io->parent);
							io->isconnect = REMOVE;
							break;
						}

						if (buffer[0] != 0x05)
						{
							debug(RELEASE, "==> detect ver[%d], but we only Support SOCKS5!\n", buffer[0]);
							io->_close(&io->parent);
							io->isconnect = REMOVE;
							break;
						}

						if (buffer[1] != 0x01)
						{
							debug(RELEASE, "==> detect CMD[%d], but we only Support CONNECT(0x01)!\n", buffer[1]);
							io->_close(&io->parent);
							io->isconnect = REMOVE;
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

						//<包装协议，转发
						struct s_header s_header;

						memset(buffer, 0, BUFFER_MAX);
						memset(&s_header, 0, sizeof(struct s_header));

						s_header.magic = 0x55AA;
						s_header.command = SOCK_CONNECT;

						memcpy(s_header.data + 0, ip, 4);
						memcpy(s_header.data + 4, port, 2);

						memcpy(buffer, &s_header, sizeof(struct s_header));

						///<transfer
						if (cb->bind_io != NULL)
							cb->bind_io->_output(&cb->bind_io->parent, buffer, sizeof(struct s_header));
					}
						break;

					case socks_state_stream:
					{
						rxnum = io->_input(&io->parent, buffer, BUFFER_MAX, TRUE);
						if (cb->bind_io != NULL)
							///<转发
							cb->bind_io->_output(&cb->bind_io->parent, buffer, rxnum);
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
			struct control_block *cb;

			debug(DEBUG, "==> client '%s'('%s')  break!\n", object_name((object_t)io), io->settings);

			ENTER_LOCK(&io->lock);

			if (io->user_ptr != NULL)
			{
				cb = (struct control_block *)io->user_ptr;
				if (cb->bind_io != NULL)
				{
					send_message(cb->bind_io->hmod, MSG_COMMAND, RST_IO, (LPARAM)cb->bind_io); 
					cb->bind_io->user_ptr = NULL;	//释放
				}
				io->user_ptr = NULL;

				free(cb);
			}

			EXIT_LOCK(&io->lock);
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
int register_thread_rcsocks_a(void)
{
    object_thread_t ot;

    ot = new_object_thread(thread_proc); 
	assert(ot);

    object_addend(&ot->parent, NAME, object_class_type_thread);

	return 0;
}
