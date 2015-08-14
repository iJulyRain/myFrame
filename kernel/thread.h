/*
 * =====================================================================================
 *
 *       Filename:  thread.h
 *
 *    Description:  thread
 *
 *        Version:  1.0
 *        Created:  06/06/2015 10:37:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __THREAD_H__
#define __THREAD_H__

#include "object.h"
#include "timer.h"
#include "message.h"
#include "poller.h"
#include "io.h"

#include <pthread.h>

typedef int (*thread_proc_t)(HMOD, int, WPARAM, LPARAM);	///<线程处理函数类型

/**
* @brief 线程模块类
*/
typedef struct object_thread
{
	struct object parent;	///<基类 

	pthread_t tid;	///<线程ID
	void *(*entry)(void *);	///<线程入口
	thread_proc_t thread_proc;	///<线程处理函数

	DWORD add_data;	///<附加参数
	struct object_information io_container;	///<线程管理的IO容器

	struct msgqueue msgqueue;	///<消息队列
}*object_thread_t;

void *thread_entry(void *parameter);
object_thread_t new_object_thread(thread_proc_t thread_proc); 
int thread_default_process(HMOD hmod, int message, WPARAM wparam, LPARAM lparam);
int start_object_thread(object_thread_t ot);
int kill_object_thread(object_thread_t ot);

#endif
