/*
 * =====================================================================================
 *
 *       Filename:  def.h
 *
 *    Description:  def
 *
 *        Version:  1.0
 *        Created:  2014年09月19日 15时10分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __DEF_H__
#define __DEF_H__

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "object.h"
#include "list.h"
#include "types.h"

#define TRUE		1
#define FALSE		0

#define ONLINE		TRUE
#define OFFLINE		FALSE

#define NOW time(NULL)

#define INIT_LOCK(lock) pthread_mutex_init(lock, NULL)
#define ENTER_LOCK(lock) pthread_mutex_lock(lock)
#define EXIT_LOCK(lock) pthread_mutex_unlock(lock)

/**
 * @brief 对象类型
 */
enum object_class_type
{
	object_class_type_thread = 0,	///<线程对象
	object_class_type_timer,		///<定时器对象
	object_class_type_io,			///<IO对象
	object_class_type_unknown
};

/**
 * @brief 对象容器类型
 */
struct object_information
{
	enum object_class_type type;	///<类型

	pthread_mutex_t lock;		///<锁

	int size;					///<长度
	list_t list;				///<链表头
};

/**
* @brief 线程模块句柄
*/
typedef UINT HMOD;

/**
* @brief 线程间消息结构
*/
typedef struct msg
{
	HMOD hmod;

	int message;

	WPARAM wparam;
	LPARAM lparam;
}*msg_t;

/**
* @brief 消息队列（环形）
*/
typedef struct msgqueue
{
	DWORD dw_data;

	int read_pos;
	int write_pos;

	sem_t wait;
	pthread_mutex_t lock;
	struct msg msg[MSGQUEUE_MAX];
}*msgqueue_t;

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

	struct msgqueue msgqueue;	///<消息队列
}*object_thread_t;

#endif
