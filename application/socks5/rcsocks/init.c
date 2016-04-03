/*
 * =====================================================================================
 *
 *       Filename:  init.c
 *
 *    Description:  app init
 *
 *        Version:  1.0
 *        Created:  06/02/2015 11:33:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *        Company:  xx
 *
 * =====================================================================================
 */
#include "common.h"

#include "rcsocks.h"	///<包含应用程序的头文件

void app_init(int argc, char **argv)
{
	if (argc < 3)
	{
		debug(RELEASE, "ERROR: invalid argument\n");
		debug(RELEASE, "run ./rcsocks 1080 1088\n");
		exit(1);
	}

	global_conf.listen_port = atoi(argv[1]);
	global_conf.reverse_port = atoi(argv[2]);

	register_thread_rcsocks_a();
	register_thread_rcsocks_b();
}
