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

#include "mem_pool_test.h"	///<包含应用程序的头文件

void app_init(int argc, char **argv)
{
	mem_pool = new_mem_pool();
	mem_pool->_info();
	mem_pool->_init(&mem_pool->parent, 0, 0, 0);	///<使用默认配置 

	register_thread_mem_pool();
}
