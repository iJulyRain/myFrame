/*
 * =====================================================================================
 *
 *       Filename:  init.c
 *
 *    Description:  init for rclient
 *
 *        Version:  1.0
 *        Created:  2016年06月25日 15时07分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (group3), lizhixian@integritytech.com.cn
 *   Organization:  
 *
 * =====================================================================================
 */

#include "common.h"

#include "rclient.h"	///<包含应用程序的头文件

void app_init(int argc, char **argv)
{
	if (argc < 3)
	{
		debug(RELEASE, "usage ./rssocks xx.xx.xx.xx:1080 50\n");
		exit(1);
	}

	global_conf.server = argv[1];
	global_conf.ncon = atoi(argv[2]);

	register_thread_rclient();
}
