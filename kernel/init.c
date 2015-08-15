/*
 * =====================================================================================
 *
 *       Filename:  init.c
 *
 *    Description:  init
 *
 *        Version:  1.0
 *        Created:  2014年09月22日 09时18分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#include "init.h"

extern struct object_information object_container[object_class_type_unknown];

/**
 * @brief 设置打印级别
 */
static void init_print_level(void)
{
	log_init();
}

/**
 * @brief 信号处理函数
 *
 * @param s 信号值
 */
static void sigvsig_handler(int s)
{
	switch (s)
	{
		case SIGSEGV:
			debug(RELEASE, "signal: SIGSEGV\n");
			break;
		case SIGTERM:
			debug(RELEASE, "signal: SIGTERM\n");
			break;
		case SIGINT:
			debug(RELEASE, "signal: SIGINT\n");
			break;
	}

	exit(1);
}

/**
 * @brief 安装信号处理函数
 */
static void install_sighandler(void)
{
	signal(SIGSEGV, sigvsig_handler);
	signal(SIGTERM, sigvsig_handler);
	signal(SIGINT,  sigvsig_handler);

	signal(SIGPIPE, SIG_IGN);
}

/**
 * @brief 注册对象
 */
static void register_class(void)
{
	///<注册系统组件
	register_all_io();
}

/**
 * @brief 启动工作线程
 *
 * @return 成功返回0，失败返回-1 
 */
static int system_threads(void)
{
	int ret;
	sem_t wait;
	pthread_t tid;
	object_thread_t ot;

	///<启动定时器线程
	debug(DEBUG, "==> start thread '%s'\n", "timer");
	sem_init(&wait, 0, 0);
	ret = pthread_create(&tid, NULL, thread_timer_entry, &wait);
	if(ret != 0)
	{
		debug(RELEASE, "==> create thread 'timer' error[%d]!\n", ret);
		return -1;
	}
	sem_wait(&wait);	///<待定时器线程启动完毕后再启动其它线程
	sem_destroy(&wait);

	///<启动应用线程
	OBJECT_FOREACH(object_class_type_thread, object_thread_t, ot)
		debug(DEBUG, "==> start thread '%s'\n", ((object_t)ot)->name);
		ret = start_object_thread(ot);
		if(ret != 0)
		{
			debug(RELEASE, "==> create thread '%s' error[%d]!\n", ((object_t)ot)->name, ret);
			return -1;
		}
	OBJECT_FOREACH_END

	return 0;
}

/**
 * @brief 初始化
 *
 * @return 成功返回0，失败返回失败的步骤
 */
int init(int argc, char **argv)
{
	init_print_level();	///< console 消息级别

	global_container_init();	///<对象容器初始化

	install_sighandler();	///<安装信号处理函数
	register_class();
	
	poller_create(POLLER_MAX);

	app_init(argc, argv);

	if(system_threads() < 0)
		return -2;

	return 0;
}
