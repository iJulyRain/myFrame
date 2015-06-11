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
#include "timer.h"

extern struct object_information object_container[object_class_type_unknown];

/**
* @brief 定时器-添加定时器 
*
* @param hmod 安装定时器的线程句柄
* @param id 定时器ID
* @param init_tick 定时器初始计数
*/
void timer_add(HMOD hmod, int id, int init_tick, void *user_data)
{
	char name[OBJ_NAME_MAX];
	object_timer_t pt = NULL;

	pt = (object_timer_t)calloc(1, sizeof(struct object_timer));
	assert(pt);

	pt->hmod = hmod;
	pt->mode = mode_timer_relative;
	pt->id = id;
	pt->init_tick = init_tick;
	pt->run = TIMER_STOP;
	pt->user_data = user_data;

	memset(name, 0, OBJ_NAME_MAX);
	sprintf(name, "%08X:%02d", hmod, id);
	
	object_addend(&pt->parent, name, object_class_type_timer);
}

static void parse_field(const char *field, uint64_t *point, int offset, int max)
{
	int i;
	char *s, *p, *saveptr;
	int begin, end, index;

	///<1
	if(strchr(field, '*') == NULL
	&& strchr(field, '-') == NULL
	&& strchr(field, ',') == NULL)
	{
		index = atoi(field);
		if(index < (max + offset))
			*point |= (1 << index);
	}
	///< '*'
	else if(strchr(field, '*') != NULL
	&& strchr(field, '-') == NULL
	&& strchr(field, ',') == NULL)
	{
		for(i = offset; i < (max + offset); i++)
			*point |= (1 << i);
	}
	///< 8-12
	else if(strchr(field, '*') == NULL
	&& strchr(field, '-') != NULL
	&& strchr(field, ',') == NULL)
	{
		s = strdup(field);

		p = strtok_r(s, "-", &saveptr);
		begin = atoi(p);
		p = strtok_r(NULL, "-", &saveptr);
		end = atoi(p);

		for(i = begin; i <= end && i < (max + offset); i++)
			*point |= (1 << i);
	}
	///< 1,3,4
	else if(strchr(field, '*') == NULL
	&& strchr(field, '-') == NULL
	&& strchr(field, ',') != NULL)
	{
		s = strdup(field);

		for(p = strtok_r(s, ",", &saveptr); p != NULL; p = strtok_r(NULL, ",", &saveptr))
		{
			index = atoi(p);
			if(index >= (max + offset))
				continue;

			*point |= (1 << index);
		}
	}
}

static void parse_timestring(const char *timestring, timerpoint_t tp)
{
	int index = 0;
	char *s, *p, *saveptr1;
	const char *field[5];
	s = strdup(timestring);

	for(p = strtok_r(s, " ", &saveptr1); p != NULL; p = strtok_r(NULL, " ", &saveptr1))
	{
		if(index >= 5)
			break;

		field[index++] = p;
	}

	parse_field(field[0], &tp->month, 1, 12);
	parse_field(field[1], &tp->day, 1, 31);
	parse_field(field[2], &tp->hour, 0, 24);
	parse_field(field[3], &tp->minute, 0, 60);
	parse_field(field[4], &tp->second, 0, 61);
}

void timer_add_abs(HMOD hmod, int id, const char *timestring, void *user_data)
{
	char name[OBJ_NAME_MAX];

	object_timer_t pt = NULL;
	struct timerpoint tp;

	pt = (object_timer_t)calloc(1, sizeof(struct object_timer));
	assert(pt);

	memset(&tp, 0, sizeof(struct timerpoint));

	parse_timestring(timestring, &tp);

	pt->hmod = hmod;
	pt->mode = mode_timer_absolutely;
	pt->id = id;
	pt->tp = tp;
	pt->user_data = user_data;

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

#if defined(USING_TIMERFD)
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
	tv.it_value.tv_nsec = 1000000000 / TICK_PER_SECOND;

	tv.it_interval = tv.it_value;

	timerfd_settime(tfd, 0, &tv, NULL);

	return tfd;
}

#elif defined(USING_SELECT)

static void timer_tick(void)
{
	struct timeval tv;

	memset(&tv, 0, sizeof(struct timeval));
	tv.tv_sec = 0;
	tv.tv_usec = 1000000 / TICK_PER_SECOND;

	select (0, NULL, NULL, NULL, &tv);
}

#else

static sem_t block;
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
	tv.it_value.tv_usec = 1000000 / TICK_PER_SECOND;	//100ms
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
	time_t now;
	struct tm tm;

#if defined(USING_TIMERFD)
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
#elif defined(USING_SELECT)
	;
#else
	init_sigalrm_timer();
#endif
	
	sem_post(wait);

	for(;;)
	{
	#if defined(USING_TIMERFD)
		ret = poll(fds, 1, 0);
		if(ret <= 0)
			continue;

		read(fds[0].fd, &tick, sizeof(long long));
		__timer_counter += tick;;
	#elif defined(USING_SELECT)
		timer_tick();
		__timer_counter ++;;
	#else
		sem_wait(&block);
	#endif

		ENTER_LOCK(&object_container[object_class_type_timer].lock);

		now = time(NULL);
		localtime_r(&now, &tm);

		OBJECT_FOREACH(object_class_type_timer, object_timer_t, pt)
			if(pt->run == TIMER_STOP)
				continue;

			if(pt->mode == mode_timer_relative)
			{
				pt->timeout_tick += (__timer_counter - old_timer_counter);
				if(pt->timeout_tick < 0)
					pt->timeout_tick = 0;

				if(pt->timeout_tick < pt->init_tick)
					continue;

				post_message(pt->hmod, MSG_TIMER, (WPARAM)pt->id, (LPARAM)pt->user_data);

				pt->timeout_tick = 0;
			}
			else if(pt->mode == mode_timer_absolutely)
			{
				if( (pt->tp.month >> (tm.tm_mon + 1) & 0x01)
				&&( (pt->tp.day >> tm.tm_mday) & 0x01 )
				&&( (pt->tp.hour >> tm.tm_hour) & 0x01 )
				&&( (pt->tp.minute >> tm.tm_min) & 0x01 )
				&&( (pt->tp.second >> tm.tm_sec) & 0x01 ) )
				{
					post_message(pt->hmod, MSG_TIMER, (WPARAM)pt->id, (LPARAM)pt->user_data);
				}
			}
		OBJECT_FOREACH_END

		EXIT_LOCK(&object_container[object_class_type_timer].lock);

		old_timer_counter = __timer_counter;
	}

	return NULL;
}
