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

#include "list.h"
#include "types.h"

#define TRUE		1
#define FALSE		0

#define ONLINE		TRUE
#define OFFLINE		FALSE

#define OBJ_NAME_MAX	32

#define MSGQUEUE_MAX	16
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
 * @brief 对象基类
 */
typedef struct object
{
	char name[OBJ_NAME_MAX];	///<基类名称
	int type;		///<对象类型
	int flag;		///<对象标志

	list_t list;	///<节点
}*object_t;

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

/**
* @brief 定时器回调函数类型
*
* @param 参数 
*
* @return NULL 
*/
typedef void (*timer_func_t)(void *parameter);

/**
* @brief 定时器类
*/
typedef struct object_timer
{
	struct object parent;	///<基类

	HMOD hmod;	///<定时器归属
	int id;		///<定时器编号
	int timeout_tick;	///<当前定时计数
	int init_tick;	///<定时器超时时间
	int run;	///<0 pause, 1 run
}*object_timer_t;

/**
* @brief 缓冲类
*/
typedef struct object_buf
{
	struct object parent;

	int buf_size;
	char *buffer;
	int read_pos, write_pos;

	char *output;

	char *head, *tail;
	size_t head_size, tail_size;
	size_t length;
}*object_buf_t;

/**
* @brief IO接口对象
*/
typedef struct object_io
{
	struct object parent;	///<基类
	HMOD hmod;

	int fd;
	int connect;
	void *settings;

	object_buf_t read_buf, send_buf;

	void (*_info)	(void);							///<接口信息
	int  (*_init)	(object_t parent, void *settings, object_buf_t, object_buf_t);	///<初始化
	int  (*_connect)(object_t parent);				///<连接
	int  (*_state)	(object_t parent);				///<检查连接
	void (*_close)	(object_t parent);				///<关闭
	int  (*_recv)	(object_t parent);	///<读
	int  (*_send)	(object_t parent);	///<写

	void *user_data;
}*object_io_t;

#endif
