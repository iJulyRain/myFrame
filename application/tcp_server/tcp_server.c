/*
 * =====================================================================================
 *
 *       Filename:  tcp_server.c
 *
 *    Description:  tcp_server
 *
 *        Version:  1.0
 *        Created:  06/13/2015 11:17:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *        Company:  xx
 *
 * =====================================================================================
 */
#include "common.h"
#include "tcp_server.h"

#include <string.h>

#define NAME "tcp server"

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

			object_io_t server;

			///<创建一个server
			server = new_object_io_tcp_server("tcp server");
			assert(server);

			server->_info();
			server->_init(&server->parent, hmod, "0.0.0.0:60001:20");
		}
			break;
		case MSG_TIMER:
		{

		}
			break;
		case MSG_COMMAND:
		{
		
		}
			break;
		case MSG_AIOCONN:
		{

		}
			break;
		case MSG_AIOIN:
		{
			object_io_t io = (object_io_t)lparam;

			if(!strcmp(object_name((object_t)io), "tcp server"))	///<表示有新连接
			{
				debug(DEBUG, "==> new client connect!\n");
			}
			else	///<有数据
			{
				int rxnum;
				char buffer[BUFFER_SIZE];

				memset(buffer, 0, BUFFER_SIZE);

				rxnum = io->_input(&io->parent, buffer, BUFFER_SIZE, TRUE);
				debug(DEBUG, "==>recv(%d): %s\n", rxnum, buffer);
			}
		}
			break;
		case MSG_AIOOUT:
		{

		}
			break;

		case MSG_AIOERR:
		{
			debug(DEBUG, "==> MSG_AIOERR\n");
		}
			break;
		case MSG_AIOBREAK:
		{
			object_io_t io = (object_io_t)lparam;

			debug(DEBUG, "==> client '%s'('%s')  break!\n", object_name((object_t)io), io->settings);

			object_delete((object_t)io);
		}
			break;
	}

	return thread_default_process(hmod, message, wparam, lparam);
}

/**
* @brief loop thread start_routine
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
int register_thread_tcp_server(void)
{
    object_thread_t ot;

    ot = (object_thread_t)calloc(1, sizeof(struct object_thread));
    assert(ot);

    ot->thread_proc = thread_proc;
    ot->entry = thread_entry;
    INIT_LOCK(&ot->msgqueue.lock);
    sem_init(&ot->msgqueue.wait, 0, 0);

	////////////
	ot->io_container.list.prev = &ot->io_container.list;
	ot->io_container.list.next = &ot->io_container.list;
	INIT_LOCK(&ot->io_container.lock);
	ot->io_container.size = 0;

    object_addend(&ot->parent, NAME, object_class_type_thread);

	return 0;
}
