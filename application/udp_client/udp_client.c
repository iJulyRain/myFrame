/*
 * =====================================================================================
 *
 *       Filename:  udp_client.c
 *
 *    Description:  udp client
 *
 *        Version:  1.0
 *        Created:  06/13/2015 11:17:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *        Company:  xx
 *
 * =====================================================================================
 */
#include "common.h"
#include "udp_client.h"

#define NAME "udp client"

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

			object_io_t client;

			///<新建一个udp client
			client = new_object_io_udp("udp client");
			assert(client);
			client->_info();
			client->_init(&client->parent, hmod, "192.168.199.172:40002");

			timer_add(hmod, 1, 1 * ONE_SECOND, client);
		}
			break;
		case MSG_TIMER:
		{
			int id = (int)wparam;

			if(id == 1)
			{
				char *msg = "test udp client\r\n";
				object_io_t client = (object_io_t)lparam;

				client->_output(&client->parent, msg, strlen(msg));
			}
		}
			break;
		case MSG_COMMAND:
		{
		
		}
			break;
		case MSG_AIOCONN:
		{
			object_io_t client = (object_io_t)lparam;

			debug(DEBUG, "==> '%s' connect to '%s' success!\n", object_name((object_t)client), client->settings);

			timer_start(hmod, 1);
		}
			break;
		case MSG_AIOIN:
		{
			debug(DEBUG, "==> MSG AIOIN!\n");

			int rxnum;
			char buffer[BUFFER_SIZE];

			object_io_t client = (object_io_t)lparam;

			memset(buffer, 0, BUFFER_SIZE);

			rxnum = client->_input(&client->parent, buffer, BUFFER_SIZE, TRUE);
			debug(DEBUG, "==>recv(%d): %s\n", rxnum, buffer);

			///<反射
			client->_output(&client->parent, buffer, rxnum);
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
			break;
		case MSG_AIOBREAK:
		{
			debug(DEBUG, "==> MSG AIOBREAK!\n");

			object_io_t client = (object_io_t)lparam;

			debug(DEBUG, "==> '%s' connect to '%s' break!\n", object_name((object_t)client), client->settings);

			timer_stop(hmod, 1);
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
int register_thread_udp_client(void)
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
