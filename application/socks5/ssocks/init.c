/*
 * =====================================================================================
 *
 *       Filename:  init.c
 *
 *    Description:  init for ssocks
 *
 *        Version:  1.0
 *        Created:  2016年05月28日 23时03分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (g3), lizhixian@integritytech.com.cn
 *   Organization:  g3
 *
 * =====================================================================================
 */
#include "common.h"

#include "ssocks.h"	///<包含应用程序的头文件

void app_init(int argc, char **argv)
{
	if (argc < 3)
	{
		debug(RELEASE, "Usage: ./rssocks 1080 20\n");
		exit(1);
	}

	global_conf.listen_port = atoi(argv[1]);
	global_conf.ncon = atoi(argv[2]);

	register_thread_ssocks();
}
