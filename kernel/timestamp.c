/*
 * =====================================================================================
 *
 *       Filename:  timestamp.c
 *
 *    Description:  timestamp
 *
 *        Version:  1.0
 *        Created:  06/06/2015 11:13:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "timestamp.h"

static void timestamp_info(void)
{
	debug(RELEASE, "==> timestamp writen by li zhixian @2015.09.05 ^.^ <==\n");
}

static int timestamp_init(object_t parent, time_t init_seconds)
{
	object_timestamp_t tms = NULL;

	tms = (object_timestamp_t)parent;
	assert(tms);

	if(init_seconds > 0)
		tms->seconds = init_seconds;
	else
		tms->seconds = time(NULL); 

	return 0;
}

static char* timestamp_fmt(object_t parent, const char *fmt)
{
	struct tm tm;
	object_timestamp_t tms = NULL;

	tms = (object_timestamp_t)parent;
	assert(tms);

	memset(&tm, 0, sizeof(struct tm));
	localtime_r(&tms->seconds, &tm);

	strftime(tms->tbuf, BUFFER_SIZE - 1, fmt, &tm);

	return tms->tbuf;
}

static void timestamp_offset(object_t parent, int offset)
{
	object_timestamp_t tms = NULL;

	tms = (object_timestamp_t)parent;
	assert(tms);
	
	tms->seconds += offset;
}

static void timestamp_set_by_time(object_t parent, time_t seconds)
{
	object_timestamp_t tms = NULL;

	tms = (object_timestamp_t)parent;
	assert(tms);
	
	tms->seconds = seconds;
}

static void timestamp_set_by_str(object_t parent, const char *fmt, const char *s)
{
	struct tm tm;
	object_timestamp_t tms = NULL;

	tms = (object_timestamp_t)parent;
	assert(tms);

	memset(&tm, 0, sizeof(struct tm));

	strptime(s, fmt, &tm);

	tms->seconds = mktime(&tm);
}

static int timestamp_get(object_t parent, int tm_type)
{
	int res;
	struct tm tm;
	object_timestamp_t tms = NULL;

	tms = (object_timestamp_t)parent;
	assert(tms);
	
	memset(&tm, 0, sizeof(struct tm));

	localtime_r(&tms->seconds, &tm);

	switch (tm_type)
	{
		case TM_YEAR:
			res = tm.tm_year + 1900;
			break;
		case TM_MON:
			res = tm.tm_mon + 1;
			break;
		case TM_DAY:
			res = tm.tm_mday;
			break;
		case TM_HOUR:
			res = tm.tm_hour;
			break;
		case TM_MIN:
			res = tm.tm_min;
			break;
		case TM_SEC:
			res = tm.tm_sec;
			break;
		case TM_WDAY:
			res = tm.tm_wday + 1;
			break;
		default:
			res = -1;
			break;
	}

	return res;
}

object_timestamp_t new_object_timestamp(void) 
{
	object_timestamp_t tms = NULL;

	tms = (object_timestamp_t)calloc(1, sizeof(struct object_timestamp));
	if(tms == NULL)
		return NULL;

	tms->_info = timestamp_info;
	tms->_init = timestamp_init;
	tms->_fmt  = timestamp_fmt;
	tms->_offset  = timestamp_offset;
	tms->_set_by_time  = timestamp_set_by_time;
	tms->_set_by_str  = timestamp_set_by_str;
	tms->_get  = timestamp_get;

	return tms;
}
