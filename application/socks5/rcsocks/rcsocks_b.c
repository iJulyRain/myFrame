/*
 * =====================================================================================
 *
 *       Filename:  rcsocks_b.c
 *
 *    Description:  rcsocks b side
 *
 *        Version:  1.0
 *        Created:  2016年04月02日 14时05分37秒
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

#define NAME "rcsocks B"

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

			object_io_t server; //listen rssocks

			///<创建一个server, 监听rssocks
			server = new_object_io_tcp_server("rcsocks B side");
			assert(server);

			memset(settings, 0, sizeof(settings));
			sprintf(settings, "0.0.0.0:%d:20", global_conf.listen_port);

			server->_info();
			server->_init(&server->parent, hmod, settings);
		}
			break;
		case MSG_AIOIN:
		{
			object_io_t io = (object_io_t)lparam;
			debug(DEBUG, "==> rcsocks B side MSG_AIOIN!\n");

			if(!strcmp(object_name((object_t)io), "rcsocks B side"))	///<表示有新连接
			{
				debug(DEBUG, "==> new client connect!\n");
			}
			else	///<有数据
			{
				int rxnum;
				char buffer[BUFFER_MAX];

				object_io_t io_stream;
				struct control_block *cb;

				memset(buffer, 0, BUFFER_MAX);
				rxnum = io->_input(&io->parent, buffer, BUFFER_MAX, TRUE);
				debug(DEBUG, "MSG_AIOIN: %d bytes!\n", rxnum);

				if (io->user_ptr == NULL)
					break;

				io_stream = (object_io_t)io->user_ptr;
				cb = (struct control_block *)io_stream->user_ptr; 

				switch (cb->state)
				{
					case socks_state_connect: //<转成sock5协议
					{
						struct s_header s_header;

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

						io_stream->_output(&io_stream->parent, buffer, 10);
						cb->state = socks_state_stream;
					}
						break;
					case socks_state_stream:
					{
						struct s_header s_header;

						memset(&s_header, 0, sizeof(struct s_header));
						memcpy(&s_header, buffer, sizeof(struct s_header));

						if((s_header.magic & 0xFFFF) == 0x55AA
						&& (s_header.command & 0xFF) == SOCK_BREAK)
						{
							io->user_ptr = NULL;
							cb->bind_io = NULL;

							io_stream->_close(&io_stream->parent);
							io_stream->isconnect = REMOVE;
						}
						else
						{
							io_stream->_output(&io_stream->parent, buffer, rxnum);
						}
					}
						break;
					default:
						break;
				}
			}
		}
			break;
		case MSG_COMMAND:
		{
			char cmd = (char)wparam;

			switch (cmd)
			{
				case GET_IO:
				{
					debug(DEBUG, "==> GET_IO\n");
					object_io_t client, *io;
					struct object_information *container;
					object_thread_t this = (object_thread_t)hmod;

					io = (object_io_t *)lparam;

					container = &this->io_container;

					CONTAINER_FOREACH(container, object_io_t, client)
						if(client->user_ptr != NULL
						|| client->mode == mode_tcp_server)
							continue;

						*io = client;
						break;
					CONTAINER_FOREACH_END
				}
					break;
				case RST_IO:
				{
					debug(DEBUG, "==> RST_IO\n");
					struct s_header s_header;
					object_io_t io = (object_io_t)lparam;
					char buffer[BUFFER_MAX];

					memset(buffer, 0, sizeof(buffer));
					memset(&s_header, 0, sizeof(struct s_header));

					s_header.magic = 0x55AA;
					s_header.command = SOCK_BREAK;
					memcpy(buffer, &s_header, sizeof(struct s_header));

					io->_output(&io->parent, buffer, sizeof(struct s_header));
				}
					break;
				default:
					break;
			}
		}
			break;
		case MSG_AIOERR:
		case MSG_AIOBREAK:
		{
			object_io_t io = (object_io_t)lparam;
			object_io_t io_stream;
			struct control_block *cb;

			debug(DEBUG, "==> client '%s'('%s')  break!\n", object_name((object_t)io), io->settings);
			if (io->user_ptr != NULL)
			{
				io_stream = (object_io_t)io->user_ptr;
				cb = (struct control_block *)io_stream->user_ptr;

				cb->bind_io = NULL;
				io->user_ptr = NULL;

				io_stream->_close(&io_stream->parent);
				io_stream->isconnect = REMOVE;
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
int register_thread_rcsocks_b(void)
{
    object_thread_t ot;

    ot = new_object_thread(thread_proc); 
	assert(ot);

    object_addend(&ot->parent, NAME, object_class_type_thread);

	return 0;
}
