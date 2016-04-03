/*
 * =====================================================================================
 *
 *       Filename:  init.c
 *
 *    Description:  init
 *
 *        Version:  1.0
 *        Created:  2016年04月02日 19时36分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (g3), lizhixian@integritytech.com.cn
 *   Organization:  g3
 *
 * =====================================================================================
 */

#include "common.h"

#include "rssocks.h"	///<包含应用程序的头文件

void app_init(int argc, char **argv)
{
	if (argc < 3)
	{
		debug(RELEASE, "ERROR: invalid argument\n");
		debug(RELEASE, "run ./rssocks xx.xx.xx.xx:20 50\n");
		exit(1);
	}

	global_conf.server = argv[1];
	global_conf.ncon = atoi(argv[2]);

	register_thread_rssocks_a();
	register_thread_rssocks_b();
}
