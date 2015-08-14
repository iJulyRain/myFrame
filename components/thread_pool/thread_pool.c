#include "thread_pool.h"

int thread_pool_proc(HMOD hmod, int message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case MSG_INIT:
		{
		}
			break;
		case MSG_TIMER:
		{
		}
			break;
		case MSG_COMMAND:
			break;
	}

	return 0;
}


int thread_pool_add_worker(object_t parent, task_func_t task_func)
{
	object_thread_pool_worker_t otpw = NULL;

	///<申请一个worker
	otpw = (object_thread_pool_worker_t)calloc(1, sizeof(struct object_thread_pool_worker));
	assert(otpw);

	otpw->thread = new_object_thread(thread_pool_proc);
	assert(otpw->thread);

	otpw->task_func = task_func;
	optw->worker_state = IDLE;
	
	///<启动worker
	start_object_thread(otpw->thread);
}

int thread_pool_remove_worker(object_t parent, object_thread_pool_worker_t worker)
{

}

object_thread_pool_worker_t thread_pool_get_idle_worker(object_t parent)
{

}

int thread_pool_assigned_task(object_thread_pool_worker_t woker, void *task, int timeout)
{

}


object_thread_pool_t new_thread_pool(int worker_max, task_func_t task_func)
{

}
