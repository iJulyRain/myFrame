/*
 * =====================================================================================
 *
 *       Filename:  rssocks_b.c
 *
 *    Description:  rssocks b side
 *
 *        Version:  1.0
 *        Created:  2016年04月02日 20时13分38秒
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

#define NAME "rssocks B"

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
		}
			break;
		case MSG_AIOIN:
		{
			debug(DEBUG, "==> rssocks B side MSG AIOIN!\n");

			int rxnum;
			char buffer[BUFFER_MAX];

			object_io_t client = (object_io_t)lparam;

			memset(buffer, 0, BUFFER_MAX);
			rxnum = client->_input(&client->parent, buffer, BUFFER_MAX, TRUE);

			object_io_t client_stream = (object_io_t)client->user_ptr;
			if(client_stream == NULL)
				break;

			client_stream->_output(&client_stream->parent, buffer, rxnum);
		}
			break;
		case MSG_AIOCONN:
		{
			object_io_t client = (object_io_t)lparam;
			object_io_t client_stream = (object_io_t)client->user_ptr;
			struct s_header s_header;
			char response[BUFFER_MAX];

			if(client_stream == NULL)
			{
				client->_close(&client->parent);
				client->isconnect = REMOVE;
				break;
			}

			debug(DEBUG, "==> %s\n", object_name(&client->parent));

			memset(&s_header, 0, sizeof(struct s_header));
			s_header.magic = 0x55AA;
			s_header.command = SOCK_CONNECT;

			if(client->_state((object_t)client) == ONLINE)
				s_header.data[0] = 0;
			else
			{
				s_header.data[0] = 1;
				client->isconnect = REMOVE;
				post_message(hmod, MSG_AIOBREAK, 0, (LPARAM)client);
			}

			memset(response, 0, sizeof(response));
			memcpy(response, &s_header, sizeof(struct s_header));

			client_stream->_output(&client_stream->parent, response, sizeof(struct s_header));
		}
			break;
		case MSG_AIOOUT:
		{
			debug(DEBUG, "==> rssocks B sidt MSG_AIOOUT! <==\n");
		}
			break;
		case MSG_AIOERR:
		case MSG_AIOBREAK:
		{
			debug(DEBUG, "==> MSG AIOBREAK!\n");

			object_io_t client = (object_io_t)lparam;
			debug(DEBUG, "==> '%s' connect to '%s' break!\n", object_name((object_t)client), client->settings);
			
			if(client->user_ptr != NULL)
			{
				object_io_t client_stream = (object_io_t)client->user_ptr;	
				client_stream->user_ptr = NULL;
				client->user_ptr = NULL;

				send_message(client_stream->hmod, MSG_COMMAND, RST_IO, (LPARAM)client_stream);
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
int register_thread_rssocks_b(void)
{
    object_thread_t ot;

    ot = new_object_thread(thread_proc); 
	assert(ot);

    object_addend(&ot->parent, NAME, object_class_type_thread);

	return 0;
}
