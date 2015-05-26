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

/**
* @brief current print level set by 'set_print_level'
*/
static int current_plevel;
static pthread_mutex_t lock;

/**
* @brief set print level to process
*
* @param plevel print level
*/
void set_print_level(int plevel)
{
	current_plevel = plevel;
	pthread_mutex_init(&lock, NULL);
}

/**
* @brief output information 
*
* @param plevel print level which set by user
* @param format format  
* @param ... ...
*
* @return always 0 
*/
int debug(int plevel, const char *format, ...)
{
	va_list ap;
	time_t now;
	char date[32];
	struct tm tm;

	now = time(NULL);
	localtime_r(&now, &tm);

	memset(date, 0, sizeof(date));
	strftime(date, 32, "%Y-%m-%d %H:%M:%S", &tm);

	pthread_mutex_lock(&lock);

	va_start(ap, format);

	if((plevel & RELEASE) >= current_plevel)
	{
		fprintf(stdout, "[ %s ] ", date);
		vprintf(format, ap);
	}

	va_end(ap);

	pthread_mutex_unlock(&lock);

	return 0;
}
