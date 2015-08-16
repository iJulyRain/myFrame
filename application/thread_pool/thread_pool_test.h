#ifndef __THREAD_POOL_X_H__
#define __THREAD_POOL_X_H__

#include <sys/time.h>

#define TASK_COMMAND_SIZE	16

typedef struct task
{
	UINT id;	///<命令编号
	int result;

	struct timeval bt, et;

	char command[TASK_COMMAND_SIZE];
}*task_t;

int register_thread_thread_pool(void);

#endif
