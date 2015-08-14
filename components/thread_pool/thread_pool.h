#ifndef __THREAD_POOL_H__ 
#define __THREAD_POOL_H__

#include "object.h"
#include "thread.h"
#include "timer.h"
#include "message.h"

enum
{
	IDLE = 1,
	BUSY,
	TIMEOUT
};

typedef int (*task_func_t)(void *);

typedef struct object_thread_pool_worker
{
	struct object parent;

	object_thread_t thread;	///<worker对应的线程 

	task_func_t task_func;	///<worker处理函数
	int worker_state;	///<worker状态
}*object_thread_pool_worker_t;

typedef struct object_thread_pool
{
	struct object parent;

	struct object_information worker_container;	///<worker的容器

	int (*add_worker)(object_t parent, task_func_t task_func);	///<新添加一个worker
	int (*remove_worker)(object_t parent, object_thread_pool_worker_t worker);	///<移除一个worker
	object_thread_pool_worker_t (*idle_worker)(object_t parent);	///<找一个空闲的worker
}*object_thread_pool_t;

///////////////////////////////////////////////////////////////////////////////
int thread_pool_add_worker(object_t parent, task_func_t task_func);
int thread_pool_remove_worker(object_t parent, object_thread_pool_worker_t worker);
object_thread_pool_worker_t thread_pool_get_idle_worker(object_t parent);

object_thread_pool_t new_thread_pool(int worker_max, task_func_t task_func); 
int thread_pool_assigned_task(object_thread_pool_worker_t woker, void *task, int timeout);

#endif
