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
#include "loop.h"

int thread_default_process(HMOD hmod, int message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case MSG_INIT:
		{
            object_thread_t ot;

            ot = (object_thread_t)hmod;
            if (ot->attr & THREAD_USING_POLLER)
                ot->poller = (poller_t)poller_create(POLLER_MAX);

			///<用于定时重连
			//timer_add(hmod, 0, 1 * ONE_SECOND, NULL, TIMER_ASYNC);
			timer_add(hmod, 0, 1, NULL, TIMER_ASYNC);
			timer_start(hmod, 0);
		}
			break;
		case MSG_TIMER:
		{
			int id = (int)wparam;

			if(id != 0)
                break;

            //debug(DEBUG, "=> timer\n");
            timer_stop(hmod, 0);

            object_thread_t this = (object_thread_t)hmod;
            object_io_t client;
            poller_t poller = NULL;
            struct object_information *container;

            container = &this->io_container;

            poller = this->poller;

            debug(DEBUG, "container->num: %d\n", container->size);

            CONTAINER_FOREACH(container, object_io_t, client)
                if(client->_state((object_t)client) != ONLINE)
                {
                    client->_connect((object_t)client);
                    if(client->_state((object_t)client) == ONLINE)
                    {
                        poller_event_setfd(client->event, client->_getfd((object_t)client));
                        poller_add((long)poller,  client->event);
                        send_message(hmod, MSG_AIOCONN, 0, (LPARAM)client);
                    }
                    else if(client->_state((object_t)client) == OFFLINE)
                    {
                        client->_close((object_t)client);
                        send_message(hmod, MSG_AIOCONN, 0, (LPARAM)client);
                    }
                }
            CONTAINER_FOREACH_END

            timer_start(hmod, 0);
		}
			break;
		case MSG_AIOERR:
		case MSG_AIOBREAK:
		{
			object_io_t client = (object_io_t)lparam;
			object_thread_t ot = (object_thread_t)hmod;

            if (client->event)
			    poller_del((long)ot->poller, client->event);

			if (io_state(&client->parent) == ONLINE)
            {
                debug(DEBUG, "==> <%s> closed \n", object_name(&client->parent));
				client->_close((object_t)client);
            }

            client->closed = TRUE;
		}
			break;
        case MSG_AIOCLR:
        {
            object_io_t client = (object_io_t)lparam;
            object_thread_t ot = (object_thread_t)hmod;

            debug(DEBUG, "==> <%s> has been removed \n", object_name(&client->parent));

            object_container_delete(&client->parent, &ot->io_container);

            free_object_io(client);

            if (client->io_pool)
                object_container_addend(&client->parent, &client->io_pool->container);
        }
            break;
		case MSG_TERM:
		{
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

    ot->poller = NULL;

	////////////
	object_container_init(&ot->io_container);

	return ot;
}

int create_thread(thread_proc_t thread_proc, const char *alias, ULONG thread_attr)
{
    object_thread_t ot;

    ot = new_object_thread(thread_proc);
    if (!ot)
        return -1;
    ot->attr = thread_attr;

    object_addend(&ot->parent, alias, object_class_type_thread);

    return 0;
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
    object_io_t client;
    struct object_information *container;

	send_message((HMOD)ot, MSG_TERM, 0, 0);	///<释放线程中可能用到的资源

    pthread_kill(ot->tid, SIGTERM);
	pthread_join(ot->tid, NULL);

    container = &ot->io_container;

    CONTAINER_FOREACH(container, object_io_t, client)
        if(client->_state((object_t)client) == ONLINE)
            client->_close((object_t)client);

        object_container_delete((object_t)client, container);
        free_object_io(client); 
        CONTAINER_FOREACH_RESET(container); ///<与delete配合使用
    CONTAINER_FOREACH_END

    DEL_LOCK(&ot->msgqueue.lock);
    sem_destroy(&ot->msgqueue.wait);

    if ((ot->attr & THREAD_USING_POLLER) && ot->poller)
    {
        poller_destroy((long)ot->poller);
        ot->poller = NULL;
    }

    free(ot);

    return 0;
}

void set_object_thread_add_data(object_thread_t ot, ULONG add_data)
{
	ot->add_data = add_data;
}

ULONG get_object_thread_add_data(object_thread_t ot)
{
	return ot->add_data;
}
