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

	rc = dzlog_init("log.ini", "category");
	if(rc)
	{
		printf("Can't load Log Config file 'log.ini'\n");
		return -1;
	}

	return 0;
}
/*
int log_init(void) 
{
	return 0;
}

void debug(int level, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vprintf(format, ap);
	va_end(ap);
}
*/
