/*
 * =====================================================================================
 *
 *       Filename:  timer.c
 *
 *    Description:  timer kernel
 *
 *        Version:  1.0
 *        Created:  2014年05月07日 11时51分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#include "common.h"

static sem_t block;

/**
* @brief 定时器-添加定时器 
*
* @param hmod 安装定时器的线程句柄
* @param id 定时器ID
* @param init_tick 定时器初始计数
*/
void timer_add(HMOD hmod, int id, int init_tick)
{
	char name[OBJ_NAME_MAX];
	object_timer_t pt = NULL;

	pt = (object_timer_t)calloc(1, sizeof(struct object_timer));
	assert(pt);

	pt->hmod = hmod;
	pt->id = id;
	pt->init_tick = init_tick;
	pt->run = TIMER_STOP;

	memset(name, 0, OBJ_NAME_MAX);
	sprintf(name, "%08X:%02d", hmod, id);
	
	object_addend(&pt->parent, name, object_class_type_timer);
}

/**
* @brief 定时器-移除定时器 
*
* @param hmod 安装定时器的线程句柄
* @param id 定时器ID
*/
void timer_remove(HMOD hmod, int id)
{
	char name[OBJ_NAME_MAX];
	object_t po;

	memset(name, 0, OBJ_NAME_MAX);
	sprintf(name, "%08X:%02d", hmod, id);

	po = object_find(name, object_class_type_timer);
	if(po == NULL)
		return;
	
	object_delete(po);

	free(po);
}

/**
* @brief 定时器-启动定时器
*
* @param hmod 安装定时器的线程句柄
* @param id 定时器ID
*/
void timer_start(HMOD hmod, int id)
{
	char name[OBJ_NAME_MAX];
	object_t p;
	object_timer_t pt;

	memset(name, 0, OBJ_NAME_MAX);
	sprintf(name, "%08X:%02d", hmod, id);

	p = object_find(name, object_class_type_timer);
	if(p == NULL)
		return;
	
	pt = (object_timer_t)p;
	pt->run = TIMER_START;
}

/**
* @brief 定时器-停止定时器 
*
* @param hmod 安装定时器的线程句柄
* @param id 定时器ID
*/
void timer_stop(HMOD hmod, int id)
{
	char name[OBJ_NAME_MAX];
	object_t p;
	object_timer_t pt;

	memset(name, 0, OBJ_NAME_MAX);
	sprintf(name, "%08X:%02d", hmod, id);

	p = object_find(name, object_class_type_timer);
	if(p == NULL)
		return;
	
	pt = (object_timer_t)p;
	pt->run = TIMER_STOP;
}

/**
* @brief 定时器-控制定时器
*
* @param hmod 安装定时器的线程句柄
* @param id 定时器ID
* @param init_tick 定时器初始计数
*/
void timer_control(HMOD hmod, int id, int init_tick)
{
	char name[OBJ_NAME_MAX];
	object_t p;
	object_timer_t pt;

	memset(name, 0, OBJ_NAME_MAX);
	sprintf(name, "%08X:%02d", hmod, id);	///<定时器命名方式：地址+ID

	p = object_find(name, object_class_type_timer);
	if(p == NULL)
		return;

	pt = (object_timer_t)p;

	pt->run = TIMER_STOP;

	pt->timeout_tick = 0;
	pt->init_tick = init_tick;

	pt->run = TIMER_START;
}

/**
* @brief 计数器
*/
static unsigned long long __timer_counter = 0;

/**
* @brief SIGALRM信号处理函数
*
* @param s 信号
*/
static void sigalrm_handler(int s)
{
	int sem_value;
	__timer_counter ++;

	sem_getvalue(&block, &sem_value);
	if(sem_value == 0)
		sem_post(&block);
}
#ifdef USING_TIMERFD
/**
* @brief timerfd方式基础定时器
*
* @return 成功返回0，失败返回-1 
*/
static int init_timerfd_timer(void)
{
	int tfd;
	struct itimerspec tv;

	tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if(tfd < 0)
	{
		debug(RELEASE, "create timerfd error!\n");
		return -1;
	}

	memset(&tv, 0, sizeof(struct itimerspec));
	tv.it_value.tv_sec = 0;
	tv.it_value.tv_nsec = 10000000;

	tv.it_interval = tv.it_value;

	timerfd_settime(tfd, 0, &tv, NULL);

	return tfd;
}

#else

/**
* @brief 信号SIGALRM方式基础定时器
*
* @return 成功返回0，失败返回-1 
*/
static int init_sigalrm_timer(void)
{
	int ret;
	struct itimerval tv;

	sem_init(&block, 0, 0);

	signal(SIGALRM, sigalrm_handler);

	tv.it_value.tv_sec = 0;
	tv.it_value.tv_usec = 100000;	//100ms
	tv.it_interval = tv.it_value;

	ret = setitimer(ITIMER_REAL, &tv, NULL);
	if(ret < 0)
	{
		debug(RELEASE, "setitimer error!\n");
		return -1;
	}

	return 0;
}

#endif

/**
* @brief 定时器线程入口
*
* @param parameter 线程参数（互斥量）
*
* @return 不会返回 
*/
void *thread_timer_entry(void *parameter)
{
	unsigned long long old_timer_counter = 0;
	object_timer_t pt;
	sem_t *wait = (sem_t *)parameter;

#ifdef USING_TIMERFD
	int ret;
	long long tick;
	struct pollfd fds[1];
	int tfd;
	
	tfd = init_timerfd_timer();
	if(tfd < 0)
		assert(0);

	memset(fds, 0, sizeof(struct pollfd));
	fds[0].fd = tfd;
	fds[0].events = POLLIN;
#else
	init_sigalrm_timer();
#endif
	
	sem_post(wait);

	for(;;)
	{
	#ifdef USING_TIMERFD
		ret = poll(fds, 1, 10);
		if(ret <= 0)
			continue;

		read(fds[0].fd, &tick, sizeof(long long));
		__timer_counter += tick;;
	#else
		sem_wait(&block);
	#endif

		ENTER_LOCK(&object_container[object_class_type_timer].lock);

		OBJECT_FOREACH(object_class_type_timer, object_timer_t, pt)
			if(pt->run == TIMER_STOP)
				continue;

			pt->timeout_tick += (__timer_counter - old_timer_counter);
			if(pt->timeout_tick < 0)
				pt->timeout_tick = 0;

			if(pt->timeout_tick < pt->init_tick)
				continue;

			post_message(pt->hmod, MSG_TIMER, pt->id, 0);

			pt->timeout_tick = 0;
		OBJECT_FOREACH_END

		EXIT_LOCK(&object_container[object_class_type_timer].lock);

		old_timer_counter = __timer_counter;
	}

	return NULL;
}
