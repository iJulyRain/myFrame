#include "thread_pool.h"

static int thread_pool_worker_proc(HMOD hmod, int message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		case MSG_INIT:
		{
			object_thread_pool_worker_t worker;
			worker = (object_thread_pool_worker_t)get_object_thread_add_data((object_thread_t)hmod);

			debug(DEBUG, "==> thread poll worker '%s' init\n", object_name((object_t)worker));

			timer_add(hmod, 1, 0, worker, TIMER_SYNC); 
			timer_add(hmod, 2, 1 * ONE_SECOND, worker, TIMER_SYNC); 
		}
			break;
		case MSG_TIMER:
		{
			int id = (int)wparam;
			object_thread_pool_worker_t worker;
			HMOD host;

			if(id == 1)
			{
				worker = (object_thread_pool_worker_t)lparam;

				if(worker->timeout == -1)	///<已经发生过超时
					break;

				debug(DEBUG, "worker '%s' task timeout!\n", object_name((object_t)worker));

				host = worker->thread_pool->hmod;

				ENTER_LOCK(&worker->lock);
				worker->worker_state = TIMEOUT;
				EXIT_LOCK(&worker->lock);

				post_message(host, MSG_STATE, (WPARAM)1, (LPARAM)worker);	///<返回结果给宿主

				worker->timeout = -1;
			}
            else if(id == 2)
            {
			    object_thread_pool_worker_t worker = (object_thread_pool_worker_t)lparam;
                worker->timeout_tick ++;
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
			
			worker->timeout = timeout;

			///<任务有超时时间
			if(timeout > 0)
			{
				timer_control(hmod, 1, timeout * ONE_SECOND);
				timer_start(hmod, 1);
			}

			timer_start(hmod, 2);

			ENTER_LOCK(&worker->lock);
			worker->worker_state = BUSY;
            worker->timeout_tick = 0;
			EXIT_LOCK(&worker->lock);

			worker->task = task;
			worker->task_func(task);	///<处理任务

			///<处理完毕
			ENTER_LOCK(&worker->lock);
			if(worker->worker_state == TIMEOUT)	///<任务已经超时了，已经反馈结果
				worker->worker_state = IDLE;
			else
			{
				worker->worker_state = IDLE;
				post_message(host, MSG_STATE, (WPARAM)0, (LPARAM)worker);	///<返回结果给宿主
			}
            worker->timeout_tick = 0;
			EXIT_LOCK(&worker->lock);

			if(timeout > 0)
				timer_stop(hmod, 1);

            timer_stop(hmod, 2);
		}
			break;
        case MSG_TERM:
        {
            timer_remove(hmod, 1);
            timer_remove(hmod, 2);
        }
            break;
	}

	return thread_default_process(hmod, message, wparam, lparam);
}

void thread_pool_info(void)
{
//	debug(RELEASE, "==> thread pool writen by li zhixian @2015.08.15 ^.^ <==\n");
}

int thread_pool_init(object_t parent, int worker_num, HMOD hmod)
{
	int i;
	object_thread_pool_t thread_pool;

	thread_pool = (object_thread_pool_t)parent;
	thread_pool->hmod = hmod;
	object_container_init(&thread_pool->worker_container);

	for(i = 0; i < worker_num; i++)
		thread_pool->add_worker(&thread_pool->parent, thread_pool->task_func);

	return 0;
}

object_thread_pool_worker_t thread_pool_add_worker(object_t parent, task_func_t task_func)
{
	object_thread_pool_t otp = NULL;
	object_thread_pool_worker_t otpw = NULL;
	int current_worker_num = 0;

	otp = (object_thread_pool_t)parent;

	current_worker_num = otp->worker_container.size;
	if(current_worker_num >= otp->worker_max)	///<线程数量超限
	{
		debug(RELEASE, "The number of threads in thread pool overrun!\n");
		return NULL;
	}

	///<申请一个worker
	otpw = (object_thread_pool_worker_t)calloc(1, sizeof(struct object_thread_pool_worker));
	if(otpw == NULL)
	{
		debug(RELEASE, "alloc memory failure!\n");
		return otpw;
	}

	otpw->thread = new_object_thread(thread_pool_worker_proc);	///<申请一个thread对象
	assert(otpw->thread);

	sprintf(otpw->parent.name, "%016lX", (ULONG)otpw);
	otpw->task_func = task_func;
	otpw->worker_state = IDLE;
	otpw->thread_pool = otp;
	INIT_LOCK(&otpw->lock);

	set_object_thread_add_data(otpw->thread, (ULONG)otpw);

	///<加入到 thread_pool
	object_container_addend(&otpw->parent, &otp->worker_container);
	
	///<启动worker
	start_object_thread(otpw->thread);

	return otpw;
}

int thread_pool_remove_worker(object_t parent, object_thread_pool_worker_t worker)
{
	object_thread_pool_t thread_pool = NULL;
	thread_pool = (object_thread_pool_t)parent;

    free_object_thread(worker->thread);
    DEL_LOCK(&worker->lock);

	object_container_delete(&worker->parent, &thread_pool->worker_container);	///<从链表钟移除

    free(worker);

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

	if(worker == NULL)	///<没有找到空闲的线程
		worker = thread_pool_add_worker(&otp->parent, otp->task_func);

	return worker;
}

void thread_pool_state(object_t parent)
{
	object_thread_pool_t otp = NULL;
	object_thread_pool_worker_t otpw = NULL;
	struct object_information *container = NULL;
	const char *state = NULL;

	otp = (object_thread_pool_t)parent;
	container = &otp->worker_container;

	debug(DEBUG, "== tid\t\t\tmagic\t\t\tstate(run) ==\n");
	debug(DEBUG, "============================================================\n");

	ENTER_LOCK(&container->lock);

	CONTAINER_FOREACH(container, object_thread_pool_worker_t, otpw)
		ENTER_LOCK(&otpw->lock);
		switch (otpw->worker_state)
		{
			case IDLE:
				state = "IDLE";
				break;
			case BUSY:
				state = "BUSY";
				break;
			case TIMEOUT:
				state = "TIMEOUT";
				break;
			default:
				state = "UNKNOWN";
				break;
		}

		EXIT_LOCK(&otpw->lock);

		debug(DEBUG, "   %lu\t%s\t%s(%d)\n",
			otpw->thread->tid,
			object_name((object_t)otpw),
			state,
            otpw->timeout_tick);
	CONTAINER_FOREACH_END
	debug(DEBUG, "\n");

	EXIT_LOCK(&container->lock);
}

int thread_pool_assigned_task(object_thread_pool_worker_t worker, void *task, int timeout)
{
	post_message((HMOD)worker->thread, MSG_COMMAND, (WPARAM)task, (LPARAM)timeout);

	return 0;
}

object_thread_pool_t new_thread_pool(int worker_max, task_func_t task_func)
{
	object_thread_pool_t thread_pool;

	thread_pool = (object_thread_pool_t)calloc(1, sizeof(struct object_thread_pool));
	assert(thread_pool);

	thread_pool->worker_max		= worker_max;
	thread_pool->task_func		= task_func;

	thread_pool->info			= thread_pool_info;
	thread_pool->state			= thread_pool_state;
	thread_pool->init			= thread_pool_init;
	thread_pool->add_worker 	= thread_pool_add_worker;
	thread_pool->remove_worker 	= thread_pool_remove_worker;
	thread_pool->idle_worker 	= thread_pool_get_idle_worker;

	return thread_pool;
}
