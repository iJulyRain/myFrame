/*
 * =====================================================================================
 *
 *       Filename:  tcp_client.c
 *
 *    Description:  tcp client
 *
 *        Version:  1.0
 *        Created:  06/13/2015 11:16:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *        Company:  xx
 *
 * =====================================================================================
 */
#include "common.h"
#include "tcp_client.h"

#define NAME "tcp client"

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

			///<新建一个tcp client
			client = new_object_io_tcp("tcp client", 0);
			assert(client);
			client->_info();
			client->_init(&client->parent, hmod, "192.168.40.113:1088");
			
			timer_add(hmod, 1, 3 * ONE_SECOND, client, TIMER_ASYNC);
		}
			break;
		case MSG_TIMER:
		{
			int id = (int)wparam;

			if(id == 1)
			{
				const char *msg = "test tcp client\r\n";

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

            if (client->isconnect == ONLINE)
            {
			    debug(DEBUG, "==> '%s' connect to '%s' success!\n", object_name((object_t)client), client->settings);
            }
            else if (client->isconnect == OFFLINE)
            {
			    debug(DEBUG, "==> '%s' connect to '%s' failed!\n", object_name((object_t)client), client->settings);
            }

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
* @brief register thread module
*
* @return always 0 
*/
int register_thread_tcp_client(void)
{
    return create_thread(thread_proc, NAME, THREAD_USING_POLLER);
    //return create_thread(thread_proc, NAME, 0);
}
