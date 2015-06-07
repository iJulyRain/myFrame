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
			timer_add(hmod, 0, 1 * TICK_PER_SECOND);
			timer_start(hmod, 0);
		}
			break;
		case MSG_TIMER:
		{
			int id = (int)wparam;

			if(id == 0)	///<connect
			{
				object_thread_t this = (object_thread_t)hmod;
				object_io_t tcp_client;
				struct object_information *container;

				container = &this->io_container;

				CONTAINER_FOREACH(container, object_io_t, tcp_client)
					if(tcp_client->mode == mode_tcp_client 
					&& tcp_client->_state(&tcp_client->parent) != ONLINE)
					{
						tcp_client->_connect(&tcp_client->parent);
						if(tcp_client->_state(&tcp_client->parent) == ONLINE)
						{
							poller_event_setfd(tcp_client->event, tcp_client->_getfd(&tcp_client->parent));
							poller_add(0,  tcp_client->event);
						}
						else if(tcp_client->_state(&tcp_client->parent) == OFFLINE)
							tcp_client->_close(&tcp_client->parent);
					}
				CONTAINER_FOREACH_END
			}
		}
			break;
		case MSG_COMMAND:
			break;
		case MSG_AIOERR:
		case MSG_AIOBREAK:
		{
			object_io_t tcp_client = (object_io_t)lparam;

			poller_del(0,  tcp_client->event);
			tcp_client->_close(&tcp_client->parent);
		}
			break;
	}

	return 0;
}

