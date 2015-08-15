#include "thread_pool.h"

static int thread_pool_worker_proc(HMOD hmod, int message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case MSG_INIT:
		{
			object_thread_pool_worker_t worker;
			worker = (object_thread_pool_worker_t)get_object_thread_add_data((object_thread_t)hmod);

			debug(DEBUG, "==> thread poll worker '%s' init\n", worker->parent.name);

			timer_add(hmod, 1, 0, worker, TIMER_SYNC); 
		}
			break;
		case MSG_TIMER:
		{
			int id = (int)wparam;
			object_thread_pool_worker_t worker;
			HMOD host;

			worker = (object_thread_pool_worker_t)lparam;
			host = worker->thread_pool->hmod;

			if(id == 1)
			{
				timer_stop(hmod, id);

				ENTER_LOCK(&worker->lock);
				worker->worker_state = TIMEOUT;
				EXIT_LOCK(&worker->lock);

				post_message(host, MSG_STATE, (WPARAM)-1, (LPARAM)worker->task);	///<返回结果给宿主
			}
		}
			break;
		case MSG_COMMAND:
		{
			void *task = (void *)wparam; 
			int timeout = (int)lparam;
			object_thread_pool_worker_t worker;
			HMOD host;

			worker = (object_thread_pool_worker_t)get_object_thread_add_data((object_thread_t)hmod);
			host = worker->thread_pool->hmod;

			///<任务有超时时间
			if(timeout > 0)
			{
				timer_control(hmod, 1, timeout * ONE_SECOND);
				timer_start(hmod, 1);
			}

			ENTER_LOCK(&worker->lock);
			worker->worker_state = BUSY;
			EXIT_LOCK(&worker->lock);

			worker->task = task;
			worker->task_func(task);	///<处理任务

			ENTER_LOCK(&worker->lock);
			if(worker->worker_state == TIMEOUT)	///<任务已经超时了，MSG_TIMER已经反馈结果
				worker->worker_state = IDLE;
			else
			{
				worker->worker_state = IDLE;
				post_message(host, MSG_STATE, (WPARAM)0, (LPARAM)task);	///<返回结果给宿主
			}
			EXIT_LOCK(&worker->lock);

			if(timeout > 0)
				timer_stop(hmod, 1);
		}
			break;
	}

	return thread_default_process(hmod, message, wparam, lparam);
}


int thread_pool_add_worker(object_t parent, task_func_t task_func)
{
	object_thread_pool_t otp = NULL;
	object_thread_pool_worker_t otpw = NULL;

	otp = (object_thread_pool_t)parent;

	///<申请一个worker
	otpw = (object_thread_pool_worker_t)calloc(1, sizeof(struct object_thread_pool_worker));
	assert(otpw);

	otpw->thread = new_object_thread(thread_pool_worker_proc);	///<申请一个thread对象
	assert(otpw->thread);

	sprintf(otpw->parent.name, "%8X", (unsigned int)otpw);
	otpw->task_func = task_func;
	otpw->worker_state = IDLE;
	otpw->thread_pool = otp;
	INIT_LOCK(&otpw->lock);

	set_object_thread_add_data(otpw->thread, (DWORD)otpw);

	///<加入到 thread_pool
	object_container_addend(&otpw->parent, &otp->worker_container);
	
	///<启动worker
	start_object_thread(otpw->thread);

	return 0;
}

int thread_pool_remove_worker(object_t parent, object_thread_pool_worker_t worker)
{
	object_thread_pool_t otp = NULL;

	otp = (object_thread_pool_t)parent;

	post_message((HMOD)worker->thread, MSG_TERM, 0, 0);	///<发送结束线程指令

	object_container_delete(&worker->parent, &otp->worker_container);	///<从链表钟移除

	return 0;
}

object_thread_pool_worker_t thread_pool_get_idle_worker(object_t parent)
{
	object_thread_pool_t otp = NULL;
	object_thread_pool_worker_t otpw, worker = NULL;
	struct object_information *container = NULL;

	otp = (object_thread_pool_t)parent;

	container = &otp->worker_container;

	ENTER_LOCK(&container->lock);

	CONTAINER_FOREACH(container, object_thread_pool_worker_t, otpw)
		ENTER_LOCK(&otpw->lock);
		if (otpw->worker_state == IDLE)
		{
			worker = otpw;

			EXIT_LOCK(&otpw->lock);
			break;
		}
		EXIT_LOCK(&otpw->lock);
	CONTAINER_FOREACH_END

	EXIT_LOCK(&container->lock);

	return worker;
}

int thread_pool_assigned_task(object_thread_pool_worker_t worker, void *task, int timeout)
{
	post_message((HMOD)worker->thread, MSG_COMMAND, (LPARAM)task, (WPARAM)timeout);

	return 0;
}

object_thread_pool_t new_thread_pool(int worker_max, task_func_t task_func, HMOD hmod)
{
	int i;
	object_thread_pool_t thread_pool;

	thread_pool = (object_thread_pool_t)calloc(1, sizeof(struct object_thread_pool));
	assert(thread_pool);

	thread_pool->hmod = hmod;

	object_container_init(&thread_pool->worker_container);

	thread_pool->add_worker 	= thread_pool_add_worker;
	thread_pool->remove_worker 	= thread_pool_remove_worker;
	thread_pool->idle_worker 	= thread_pool_get_idle_worker;

	for(i = 0; i < worker_max; i++)
		thread_pool_add_worker(&thread_pool->parent, task_func);

	return thread_pool;
}
