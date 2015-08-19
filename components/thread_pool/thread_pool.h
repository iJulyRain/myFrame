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

typedef struct object_thread_pool_worker *object_thread_pool_worker_t;
typedef struct object_thread_pool *object_thread_pool_t;

struct object_thread_pool_worker
{
	struct object parent;

	object_thread_t thread;	///<worker对应的线程 

	task_func_t task_func;	///<worker处理函数
	void *task;	///<处理的任务

	/*
	 * -1 有超时，且已超时
	 *	0 无超时
	 *	> 0 超时时长
	 */
	int timeout;	///<任务超时时长 单位秒
	int worker_state;	///<worker状态

	pthread_mutex_t lock;

	object_thread_pool_t thread_pool;	///<worker所属的thread pool
};

struct object_thread_pool
{
	struct object parent;

	HMOD hmod;	///<thread pool所属的HMOD

	int worker_max;	///<线程池允许创建的最大线程数
	struct object_information worker_container;	///<worker的容器

	task_func_t task_func;

	void (*info)(void);
	void (*state)(object_t parent);
	int (*init)(object_t parent, int worker_num, HMOD hmod);	///<线程池初始化，会预先创建worker_num个线程出来
	object_thread_pool_worker_t (*add_worker)(object_t parent, task_func_t task_func);	///<新添加一个worker
	int (*remove_worker)(object_t parent, object_thread_pool_worker_t worker);	///<移除一个worker
	object_thread_pool_worker_t (*idle_worker)(object_t parent);	///<找一个空闲的worker
};

///////////////////////////////////////////////////////////////////////////////
object_thread_pool_t new_thread_pool(int worker_max, task_func_t task_func);

int thread_pool_assigned_task(object_thread_pool_worker_t worker, void *task, int timeout);

#endif
