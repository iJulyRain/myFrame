#include "common.h"
#include "thread_pool_test.h"

#define NAME "thread pool"

static UINT global_id = 0;

static int task_func(void *t)
{
	pthread_t self;
	task_t task = (task_t)t;

	self = pthread_self();

	debug(DEBUG, "==> in thread %lu, command: %s\n", self, task->command);
	sleep(2);

	task->result = 0;

	return 0;
}

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
			object_thread_pool_t thread_pool;

			debug(DEBUG, "### '%s'\tMSG_INIT\n", NAME);

			thread_pool = new_thread_pool(30, task_func); 
			thread_pool->info();
			thread_pool->init(&thread_pool->parent, 10, hmod);

			timer_add(hmod, 1, 1 * ONE_SECOND, thread_pool, TIMER_ASYNC);
			timer_start(hmod , 1);

			timer_add(hmod, 2, 5, thread_pool, TIMER_ASYNC);
			timer_start(hmod , 2);

			srand((unsigned int)time(NULL));
		}
			break;
		case MSG_TIMER:
		{
			int id = (int)wparam;
			object_thread_pool_t thread_pool = NULL;
			object_thread_pool_worker_t worker = NULL;

			if(id == 1)
			{
				thread_pool = (object_thread_pool_t)lparam;

				thread_pool->state(&thread_pool->parent);
			}
			else if(id == 2)
			{
				task_t task = (task_t)calloc(1, sizeof(struct task));
				assert(task);

				thread_pool = (object_thread_pool_t)lparam;

				task->id = global_id++;
				snprintf(task->command, TASK_COMMAND_SIZE - 1, "cmd: %d", rand() % 10);

				worker = thread_pool->idle_worker(&thread_pool->parent);
				if(worker == NULL)
					break;

				thread_pool_assigned_task(worker, (void *)task, 0);
			}
		}
			break;
		case MSG_COMMAND:
		{
		
		}
			break;
		case MSG_STATE:
		{
			task_t task = (task_t)lparam;

			debug(DEBUG, "==> [%d] task'id '%d' result: %d\n", (int)wparam, task->id, task->result);

			free(task);
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
int register_thread_thread_pool(void)
{
    object_thread_t ot;

    ot = new_object_thread(thread_proc); 
	assert(ot);

    object_addend(&ot->parent, NAME, object_class_type_thread);

	return 0;
}
