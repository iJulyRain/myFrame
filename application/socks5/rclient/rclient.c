/*
 * =====================================================================================
 *
 *       Filename:  rclient.c
 *
 *    Description:  rclient
 *
 *        Version:  1.0
 *        Created:  2016年06月25日 15时10分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (group3), lizhixian@integritytech.com.cn
 *   Organization:  
 *
 * =====================================================================================
 */

#include "common.h"

#include "rclient.h"

#define NAME "R CLIENT"

static object_io_pool_t io_pool; ///<connect to target 

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
            struct control_block *cb;

            ///<clients connect to rserver
			for(i = 0; i < global_conf.ncon; i++)
			{
				memset(name, 0, sizeof(name));
				snprintf(name, 31, "[%d] rserver client", i);

				client = new_object_io_tcp(name, 0);
				assert(client);
				client->_info();
				client->_init(&client->parent, hmod, global_conf.server);

                cb = (struct control_block *)calloc(1, sizeof(struct control_block));
                assert(cb);
                cb->io_bind = NULL;
                client->user_ptr = cb;
			}

            io_pool = new_io_pool("tcp client", IO_POOL_MAX, mode_tcp_client);
            assert(io_pool);
            io_pool->_init(&io_pool->parent);

			timer_add(hmod, 1, 60 * ONE_SECOND, NULL, TIMER_ASYNC); ///<HEART BEAT
			timer_start(hmod, 1);
		}
			break;
        case MSG_TIMER:
        {
            int id = (int)wparam;

            if (id != 1)
                break;

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
                if (strstr(object_name(&client->parent), "rserver client"))
                    client->_output(&client->parent, buffer, sizeof(struct s_header));
            CONTAINER_FOREACH_END
        }
            break;

        case MSG_AIOCONN:
        {
			object_io_t client, io_bind; 
            struct control_block *cb;
			struct s_header s_header;
			char response[128];

            client = (object_io_t)lparam;
            cb = (struct control_block *)client->user_ptr;

            debug(DEBUG, "==> <%s> connected status %d\n", object_name(&client->parent), client->_state(&client->parent));

            if (strstr(object_name(&client->parent), "rserver client")) ///<connect to rserver
                break;

            io_bind = cb->io_bind;
            if (io_bind == NULL)
                break;

            memset(&s_header, 0, sizeof(struct s_header));
            s_header.magic = 0x55AA;
            s_header.command = SOCK_CONNECT;

            if(client->_state((object_t)client) == ONLINE)
            {
                debug(DEBUG, " %s <==> %s success\n", object_name(&io_bind->parent), object_name(&client->parent));
                s_header.data[0] = 0;
            }
            else
            {
                debug(DEBUG, " %s <==> %s failed\n", object_name(&io_bind->parent), object_name(&client->parent));
                s_header.data[0] = 1;

                ///<未加入到poller，手动删除
                post_message(hmod, MSG_AIOBREAK, 0, (LPARAM)client);
                post_message(hmod, MSG_AIOCLR, 0, (LPARAM)client);
            }

            memset(response, 0, sizeof(response));
            memcpy(response, &s_header, sizeof(struct s_header));

            io_bind->_output(&io_bind->parent, response, sizeof(struct s_header));
        }
            break;

		case MSG_AIOIN:
		{
			int rxnum;
			char buffer[BUFFER_SIZE];

			object_io_t client, io_bind;
            struct control_block *cb;
			struct s_header s_header;

			client = (object_io_t)lparam;

            memset(buffer, 0, sizeof(buffer));
			rxnum = client->_input(&client->parent, buffer, BUFFER_SIZE, TRUE);
			//debug(DEBUG, "==> rxnum: %d bytes\n", rxnum);

            if (strstr(object_name(&client->parent), "rserver client")) ///<from rserver
            {
                memset(&s_header, 0, sizeof(struct s_header));
                memcpy(&s_header, buffer, sizeof(struct s_header));

                if ((s_header.magic & 0xFFFF) == 0x55AA) //<command from server
                {
                    switch (s_header.command & 0xFF)
                    {
                        case SOCK_BREAK: //< close
                        {
                            cb = (struct control_block *)client->user_ptr;
                            io_bind = cb->io_bind; 
                            if (io_bind != NULL)
                                io_bind->_close(&io_bind->parent);
                        }
                            break;
                        case SOCK_HEART:
                        {
                            debug(DEBUG, "==> HEART BEAT\n");
                        }
                            break;
                        case SOCK_CONNECT: //< connect
                        {
                            char addr[6]; //<ip and port
                            char settings[32];
                            uint16_t port;
                            struct sockaddr_in saddr;

                            //<解析 IP和端口
                            memcpy(addr, s_header.data, 6);

                            memset(&saddr, 0, sizeof(struct sockaddr_in));
                            memcpy(&saddr.sin_addr.s_addr, addr, 4);

                            port = addr[4] << 8 | addr[5];

                            memset(settings, 0, sizeof(settings));
                            sprintf(settings, "%s:%d", inet_ntoa(saddr.sin_addr), port);

                            debug(DEBUG, "==> new client <---> %s\n", settings);

                            ///<connect to target
                            io_bind = (object_io_t)io_pool->_get_one(&io_pool->parent);
                            assert(io_bind);

                            cb = (struct control_block *)io_bind->user_ptr;
                            if (cb == NULL)
                            {
                                cb = (struct control_block *)calloc(1, sizeof(struct control_block));
                                assert(cb);
                            }

                            cb->io_bind = client;
                            io_bind->user_ptr = cb;

                            cb = (struct control_block *)client->user_ptr;
                            assert(cb);
                            cb->io_bind = io_bind;

                            debug(DEBUG, "==> bind %s <---> %s\n", object_name(&client->parent), object_name(&io_bind->parent));

                            io_bind->_info();
                            io_bind->_init(&io_bind->parent, hmod, settings);
                            //io_bind->_connect(&io_bind->parent);    ///<立即连接
                        }
                            break;
                    }

                    break;
                }
            }

            ///<transfer
            cb = (struct control_block *)client->user_ptr;
            io_bind = cb->io_bind; 
            if(io_bind == NULL)
                break;

            io_bind->_output(&io_bind->parent, buffer, rxnum);
		}
			break;

		case MSG_AIOERR:
		case MSG_AIOBREAK:
		{
			object_io_t client, io_bind;
            struct control_block *cb; 

            client = (object_io_t)lparam;

            debug(DEBUG, "MSG_AIOBREAK <%s>\n", object_name(&client->parent));

            cb = (struct control_block *)client->user_ptr;
            io_bind = cb->io_bind;
            if (io_bind == NULL)
                break;

            if (strstr(object_name(&client->parent), "rserver client"))
            {
                io_bind->_close(&io_bind->parent);
            }
            else
            {
                char buffer[128];
                struct s_header s_header;
                
                memset(&s_header, 0, sizeof(struct s_header));

                s_header.magic = 0x55AA;
                s_header.command = SOCK_BREAK;

                memset(buffer, 0, sizeof(buffer));
                memcpy(buffer, &s_header, sizeof(struct s_header));

                io_bind->_output(&io_bind->parent, buffer, sizeof(struct s_header));
            }

            cb->io_bind = NULL;

            cb = (struct control_block *)io_bind->user_ptr;
            if (cb != NULL)
                cb->io_bind = NULL;
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
int register_thread_rclient(void)
{
    return create_thread(thread_proc, NAME, THREAD_USING_POLLER);
}
