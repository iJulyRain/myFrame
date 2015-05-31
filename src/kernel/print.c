/*
 * =====================================================================================
 *
 *       Filename:  print.c
 *
 *    Description:  print
 *
 *        Version:  1.0
 *        Created:  2014年05月07日 11时53分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#include "print.h"

int log_init(void) 
{
	int rc;

	rc = zlog_init("config/log.ini");
	if(rc)
	{
		printf("Step1. Can't load Log Config file 'config/log.ini'\n");
		return -1;
	}

	zc = zlog_get_category("category");
	if(!zc)
	{
		printf("Step2. Can't load Category!\n");
		return -2;
	}

	return 0;
}
