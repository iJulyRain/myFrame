/*
 * =====================================================================================
 *
 *       Filename:  rssocks_a.c
 *
 *    Description:  rssocks a side
 *
 *        Version:  1.0
 *        Created:  2016年04月02日 20时13分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (g3), lizhixian@integritytech.com.cn
 *   Organization:  g3
 *
 * =====================================================================================
 */

#include "common.h"
#include "rssocks.h"

#define NAME "rssocks A"

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
			int i;
			char name[32];
			object_io_t client;

			for(i = 0; i < global_conf.ncon; i++)
			{
				memset(name, 0, sizeof(name));
				snprintf(name, 31, "[%d] tcp client(A)", i);

				client = new_object_io_tcp(name, 0);
				assert(client);
				client->_info();
				client->_init(&client->parent, hmod, global_conf.server);
			}

			timer_add(hmod, 1, 60 * ONE_SECOND, NULL, TIMER_ASYNC);
			timer_start(hmod, 1);
		}
			break;
        case MSG_TIMER:
        {
            int id = (int)wparam;

            switch(id)
            {
                case 1:
                {
			        struct s_header s_header;
                    char buffer[128];
                    struct object_information *container;
                    object_io_t client;

                    object_thread_t this = (object_thread_t)hmod;

                    memset(&s_header, 0, sizeof(struct s_header));
                    s_header.magic = 0x55AA; 
                    s_header.command = SOCK_HEART;

                    memset(buffer, 0, sizeof(buffer));
                    memcpy(buffer, &s_header, sizeof(struct s_header));
                    
                    container = &this->io_container;

                    CONTAINER_FOREACH(container, object_io_t, client)
                        if(client->_state(&client->parent) != ONLINE
                        || client->mode == mode_tcp_server)
                            continue;

                        client->_output(&client->parent, buffer, sizeof(struct s_header));
                    CONTAINER_FOREACH_END
                }
                    break;
            }
        }
            break;
		case MSG_AIOIN: //<data from rcsocks B side
		{
			debug(DEBUG, "==> MSG AIOIN!\n");

			int rxnum;
			char buffer[BUFFER_MAX];
			object_io_t client;
			struct s_header s_header;

			object_thread_t this = (object_thread_t)hmod;
			struct object_information *container;
			container = &this->io_container;

			int cnt = 0;
			CONTAINER_FOREACH(container, object_io_t, client)
				if (client->user_ptr != NULL)
					cnt++;
			CONTAINER_FOREACH_END

			debug(DEBUG, "==> client pool used %d/%d\n", cnt, global_conf.ncon);

			client = (object_io_t)lparam;

            memset(buffer, 0, sizeof(buffer));
			rxnum = client->_input(&client->parent, buffer, BUFFER_MAX, TRUE);
			debug(DEBUG, "==> rxnum: %d bytes\n", rxnum);

			memset(&s_header, 0, sizeof(struct s_header));
			memcpy(&s_header, buffer, sizeof(struct s_header));

			if ((s_header.magic & 0xFFFF) == 0x55AA) //<command from server
			{
				switch (s_header.command & 0xFF)
				{
					case SOCK_BREAK: //< close
					{
						debug(DEBUG, "==> close client!\n");
						if (client->user_ptr != NULL)
						{
							object_io_t client_stream = (object_io_t)client->user_ptr;

							client->user_ptr = NULL;
							client_stream->user_ptr = NULL;

							client_stream->_close(&client_stream->parent);
							client_stream->isconnect = REMOVE;
						}
					}
						break;
					case SOCK_CONNECT: //< connect
					{
						char addr[6]; //<ip and port
						char settings[32];
						uint16_t port;
						struct sockaddr_in saddr;
						object_thread_t ot = NULL;
						object_io_t client_stream = NULL;

						//<解析 IP和端口
						memcpy(addr, s_header.data, 6);

						memset(&saddr, 0, sizeof(struct sockaddr_in));
						memcpy(&saddr.sin_addr.s_addr, addr, 4);

						port = addr[4] << 8 | addr[5];

						memset(settings, 0, sizeof(settings));
						sprintf(settings, "%s:%d", inet_ntoa(saddr.sin_addr), port);

						debug(DEBUG, "==> new client <---> %s\n", settings);

						///<创建连接
						ot = (object_thread_t)object_find("rssocks B", object_class_type_thread);
						assert(ot);

						client_stream = new_object_io_tcp(settings, IO_ATTR_REMOVE);
						assert(client_stream);

						client->user_ptr = client_stream;
						client_stream->user_ptr = client;

						client_stream->_info();
						client_stream->_init(&client_stream->parent, (HMOD)ot, settings); //<托管在rssocks B 线程
						client_stream->mode |= IO_REMOVE; //<断开后删除

						client_stream->_connect(&client_stream->parent);
					}
						break;
				}
			}
			else
			{
				object_io_t client_stream = (object_io_t)client->user_ptr;
				if(client_stream == NULL)
					break;

				client_stream->_output(&client_stream->parent, buffer, rxnum);
			}
		}
			break;
		case MSG_COMMAND:
		{
			char cmd = (char)wparam;
			switch (cmd)
			{
				case RST_IO:
				{
					debug(DEBUG, "==> RST_IO\n");

					object_io_t io = (object_io_t)lparam;
					char buffer[128];
					struct s_header s_header;
                    
					memset(&s_header, 0, sizeof(struct s_header));

					s_header.magic = 0x55AA;
					s_header.command = SOCK_BREAK;

                    memset(buffer, 0, sizeof(buffer));
					memcpy(buffer, &s_header, sizeof(struct s_header));

					io->_output(&io->parent, buffer, sizeof(struct s_header));
				}
					break;
				default:
					break;
			}
		}
			break;
		case MSG_AIOOUT:
		{
			debug(DEBUG, "==> rssocks A side MSG AIOOUT<==\n");
		}
			break;
		case MSG_AIOERR:
		case MSG_AIOBREAK:
		{
			debug(DEBUG, "==> MSG AIOBREAK!\n");

			object_io_t client = (object_io_t)lparam;
			debug(DEBUG, "==> '%s' connect to '%s' break!\n", object_name((object_t)client), client->settings);

			if (client->user_ptr != NULL)
			{
				object_io_t client_stream = (object_io_t)client->user_ptr;

				client->user_ptr = NULL;
				client_stream->user_ptr = NULL;

				client_stream->_close(&client_stream->parent);
				client_stream->isconnect = REMOVE;
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
int register_thread_rssocks_a(void)
{
    object_thread_t ot;

    ot = new_object_thread(thread_proc); 
	assert(ot);

    object_addend(&ot->parent, NAME, object_class_type_thread);

	return 0;
}
