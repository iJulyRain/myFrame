/*
 * =====================================================================================
 *
 *       Filename:  daemon.c
 *
 *    Description:  daemon
 *
 *        Version:  1.0
 *        Created:  2014年09月22日 09时51分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#include "common.h"

#include "daemon.h"

#define NAME "daemon"

/**
* @brief daemon loop process 
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

			object_thread_t this = (object_thread_t)hmod;

			///<新建一个tcp client
			object_io_t tcp_client = new_object_io("io tcp", "tcp client");
			assert(tcp_client);
			tcp_client->_info();
			tcp_client->_init(&tcp_client->parent, hmod, "192.168.1.138:40001");
			object_container_addend(&tcp_client->parent, &this->io_container);	///<填充到线程的IO容器里面

			///<用于定时重连
			timer_add(hmod, 1, 1 * TICK_PER_SECOND);
			timer_start(hmod, 1);
		}
			break;
		case MSG_TIMER:
		{
			int id = (int)wparam;

			debug(DEBUG, "MSG TIMER %d\n", id);

			object_thread_t this = (object_thread_t)hmod;
			if(id == 1)	///<connect
			{
				object_io_t tcp_client;

				tcp_client = (object_io_t)object_container_find("tcp client", &this->io_container);
				if(tcp_client->_connect(&tcp_client->parent) == ONLINE)
				{
					timer_stop(hmod, id);	///<连接成功，关闭定时器

					poller_event_setfd(tcp_client->event, tcp_client->_getfd(&tcp_client->parent));
					poller_add(0,  tcp_client->event);
				}
				else
					tcp_client->_close(&tcp_client->parent);
			}
		}
			break;
		case MSG_COMMAND:
		{
		
		}
			break;
		case MSG_AIOIN:
		{
			debug(DEBUG, "==> MSG AIOIN!\n");

			int rxnum;
			char buffer[BUFFER_SIZE];
			object_thread_t this = (object_thread_t)hmod;
			object_io_t tcp_client = (object_io_t)object_container_find("tcp client", &this->io_container);

			memset(buffer, 0, BUFFER_SIZE);

			rxnum = tcp_client->_input(&tcp_client->parent, buffer, BUFFER_SIZE, TRUE);
			debug(DEBUG, "==>recv(%d): %s\n", rxnum, buffer);

			tcp_client->_output(&tcp_client->parent, buffer, rxnum);
		}
			break;
		case MSG_AIOOUT:
		{
			debug(DEBUG, "==> write complete!\n");
		}
			break;
		case MSG_AIOERR:
		{
			debug(DEBUG, "==> MSG AIOERR!\n");
		}
		case MSG_AIOBREAK:
		{
			debug(DEBUG, "==> MSG AIOBREAK!\n");

			object_thread_t this = (object_thread_t)hmod;
			object_io_t tcp_client = (object_io_t)object_container_find("tcp client", &this->io_container);

			poller_del(0,  tcp_client->event);
			tcp_client->_close(&tcp_client->parent);

			timer_start(hmod, 1);
		}
			break;

		break;
	}

	return 0;
}

/**
* @brief daemon loop thread start_routine
*
* @param data argument of start routine_
*
* @return NULL, but never return 
*/
static void *thread_entry(void *parameter)
{
    struct msg msg;
    HMOD hmod;

    hmod = (HMOD)parameter; 

    while(!get_message(hmod, &msg))
        dispatch_message(&msg);

	return NULL;
}


/**
* @brief register thread module
*
* @return always 0 
*/
int register_thread_daemon(void)
{
    object_thread_t ot;

    ot = (object_thread_t)calloc(1, sizeof(struct object_thread));
    assert(ot);

    ot->thread_proc = thread_proc;
    ot->entry = thread_entry;
    INIT_LOCK(&ot->msgqueue.lock);
    sem_init(&ot->msgqueue.wait, 0, 0);

	ot->io_container.list.prev = &ot->io_container.list;
	ot->io_container.list.next = &ot->io_container.list;
	INIT_LOCK(&ot->io_container.lock);
	ot->io_container.size = 0;

    object_addend(&ot->parent, NAME, object_class_type_thread);

	return 0;
}
