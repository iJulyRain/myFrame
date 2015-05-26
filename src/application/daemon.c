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

			timer_add(hmod, 1, 1 * TICK_PER_SECOND);	///<12
			timer_start(hmod, 1);
		}
			break;
		case MSG_TIMER:
		{
			debug(DEBUG, "MSG TIMER\n");
		}

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
    pthread_mutex_init(&ot->msgqueue.lock, NULL);
    sem_init(&ot->msgqueue.wait, 0, 0);

    object_addend(&ot->parent, NAME, object_class_type_thread);

	return 0;
}
