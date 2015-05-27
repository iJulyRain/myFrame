/*
 * =====================================================================================
 *
 *       Filename:  timer.h
 *
 *    Description:  timer
 *
 *        Version:  1.0
 *        Created:  2014年09月22日 16时42分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __TIMER_H__
#define __TIMER_H__

#ifdef USING_TIMERFD
#include <sys/timefd.h>
#endif

#include "def.h"

#define TIMER_STOP		0
#define TIMER_START		1

#define TICK_PER_SECOND 10

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

void timer_add(HMOD hmod, int id, int init_tick);
void timer_remove(HMOD hmod, int id);
void timer_start(HMOD hmod, int id);
void timer_stop(HMOD hmod, int id);
void timer_control(HMOD hmod, int id, int init_tick);

void *thread_timer_entry(void *parameter);

#endif
