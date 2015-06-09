/*
 * =====================================================================================
 *
 *       Filename:  thread.c
 *
 *    Description:  thread
 *
 *        Version:  1.0
 *        Created:  06/06/2015 10:37:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "thread.h"

int thread_default_process(HMOD hmod, int message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case MSG_INIT:
		{
			///<用于定时重连
			timer_add(hmod, 0, 1 * TICK_PER_SECOND, NULL);
			timer_start(hmod, 0);
		}
			break;
		case MSG_TIMER:
		{
			int id = (int)wparam;

			if(id == 0)	///<connect
			{
				timer_stop(hmod, 0);

				object_thread_t this = (object_thread_t)hmod;
				object_io_t client;
				struct object_information *container;

				container = &this->io_container;

				CONTAINER_FOREACH(container, object_io_t, client)
					if(client->mode == mode_tcp_client 
					&& client->_state((object_t)client) != ONLINE)
					{
						client->_connect((object_t)client);
						if(client->_state((object_t)client) == ONLINE)
						{
							poller_event_setfd(client->event, client->_getfd((object_t)client));
							poller_add(0,  client->event);
							send_message(hmod, MSG_AIOCONN, 0, (LPARAM)client);
						}
						else if(client->_state((object_t)client) == OFFLINE)
							client->_close((object_t)client);
					}
				CONTAINER_FOREACH_END

				timer_start(hmod, 0);
			}
		}
			break;
		case MSG_COMMAND:
			break;
		case MSG_AIOERR:
		case MSG_AIOBREAK:
		{
			object_io_t client = (object_io_t)lparam;

			poller_del(0,  client->event);
			client->_close((object_t)client);
		}
			break;
	}

	return 0;
}

