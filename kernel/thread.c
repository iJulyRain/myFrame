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
			timer_add(hmod, 0, 1 * ONE_SECOND, NULL, TIMER_ASYNC);
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
					if(client->_state((object_t)client) != ONLINE)
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
		case MSG_AIOERR:
		case MSG_AIOBREAK:
		{
			object_io_t client = (object_io_t)lparam;

			poller_del(0,  client->event);
			client->_close((object_t)client);
		}
			break;
		case MSG_TERM:
		{
            object_thread_t this = (object_thread_t)hmod;
            object_io_t client;
            struct object_information *container;

            container = &this->io_container;

            CONTAINER_FOREACH(container, object_io_t, client)
                if(client->_state((object_t)client) == ONLINE)
                    client->_close((object_t)client);

                object_container_delete((object_t)client, container);
                free_object_io(client); 
                CONTAINER_FOREACH_RESET(container); ///<与delete配合使用
            CONTAINER_FOREACH_END

            timer_remove(hmod, 0);
		}
			break;
	}

	return 0;
}

void *thread_entry(void *parameter)
{
    struct msg msg;
    HMOD hmod;

    hmod = (HMOD)parameter; 
    
//    pthread_detach(pthread_self());

    while(!get_message(hmod, &msg))
        dispatch_message(&msg);

	return NULL;
}

object_thread_t new_object_thread(thread_proc_t thread_proc) 
{
    object_thread_t ot;

    ot = (object_thread_t)calloc(1, sizeof(struct object_thread));
	if(ot == NULL)
		return NULL;

    ot->thread_proc = thread_proc;
    ot->entry = thread_entry;
    INIT_LOCK(&ot->msgqueue.lock);
    sem_init(&ot->msgqueue.wait, 0, 0);

	////////////
	object_container_init(&ot->io_container);

	return ot;
}

void free_object_thread(object_thread_t ot)
{
    DEL_LOCK(&ot->msgqueue.lock);
    sem_destroy(&ot->msgqueue.wait);

    object_container_deinit(&ot->io_container);
}

int start_object_thread(object_thread_t ot)
{
	send_message((HMOD)ot, MSG_INIT, 0, 0);

	return pthread_create(&ot->tid, NULL, ot->entry, (void *)ot);
}

int kill_object_thread(object_thread_t ot)
{
	send_message((HMOD)ot, MSG_TERM, 0, 0);	///<释放线程中可能用到的资源

    pthread_kill(ot->tid, SIGTERM);

	return pthread_join(ot->tid, NULL);
}

void set_object_thread_add_data(object_thread_t ot, ULONG add_data)
{
	ot->add_data = add_data;
}

ULONG get_object_thread_add_data(object_thread_t ot)
{
	return ot->add_data;
}
