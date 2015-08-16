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

#include "thread_pool_test.h"	///<包含应用程序的头文件

void app_init(int argc, char **argv)
{
	register_thread_thread_pool();
}
