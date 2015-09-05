#include "common.h"
#include "timestamp_test.h"

#define NAME "timestamp"

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

			object_timestamp_t tms = new_object_timestamp();
			assert(tms);

			tms->_info();
			tms->_init(&tms->parent, 0);
			tms->_set_by_str(&tms->parent, TM_STR_FMT1, "2012-12-25 03:01:33");

			timer_add(hmod, 1, 1 * ONE_SECOND, tms, TIMER_ASYNC);
			timer_start(hmod, 1);
		}
			break;
		case MSG_TIMER:
		{
			int id = (int)wparam;
			char *t;

			if(id == 1)
			{
				object_timestamp_t tms = (object_timestamp_t)lparam;
				t = tms->_fmt(&tms->parent,TM_STR_FMT1);
				debug(DEBUG, "==>timestring: %s\n", t);

				tms->_offset(&tms->parent, 1);
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
int register_thread_timestamp(void)
{
    object_thread_t ot;

    ot = new_object_thread(thread_proc); 
	assert(ot);

    object_addend(&ot->parent, NAME, object_class_type_thread);

	return 0;
}
