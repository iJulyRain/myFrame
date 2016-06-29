/*
 * =====================================================================================
 *
 *       Filename:  print.h
 *
 *    Description:  debug interface 
 *
 *        Version:  1.0
 *        Created:  2014年09月19日 15时13分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __PRINT_H__
#define __PRINT_H__

#include <zlog.h>

//print level
#define DEBUG 0	///<print level debug
#define RELEASE 1 ///<print level release
#define ERROR 1

/**
* @brief logger initialize 
* 0 success, 01 failed
*/
int log_init(void);

void debug(int level, const char *format, ...);

#define debug(level, format, arg...)	\
	{	\
		if(level == DEBUG) \
			dzlog_debug(format, ##arg);\
		else if(level == RELEASE)\
			dzlog_error(format, ##arg);\
	}

#define vdebug(level, format, arg)	\
	{	\
		if(level == DEBUG) \
			vdzlog_debug(format, arg);\
		else if(level == RELEASE)\
			vdzlog_error(format, arg);\
	}

#endif
