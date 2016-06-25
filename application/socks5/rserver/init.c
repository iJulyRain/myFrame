/*
 * =====================================================================================
 *
 *       Filename:  init.c
 *
 *    Description:  init for server
 *
 *        Version:  1.0
 *        Created:  2016年06月23日 14时59分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (group3), lizhixian@integritytech.com.cn
 *   Organization:  
 *
 * =====================================================================================
 */

#include "common.h"

#include "rserver.h"	///<包含应用程序的头文件

void app_init(int argc, char **argv)
{
	if (argc < 3)
	{
		debug(RELEASE, "usage: ./server 1080 1088\n");
		exit(1);
	}

	global_conf.listen_port = atoi(argv[1]);
	global_conf.reverse_port = atoi(argv[2]);

	register_thread_rserver();
}
