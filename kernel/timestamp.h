/*
 * =====================================================================================
 *
 *       Filename:  timestamp.h
 *
 *    Description:  timestamp
 *
 *        Version:  1.0
 *        Created:  06/06/2015 11:13:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include "object.h"
#include "print.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ONE_MINUTE	60
#define ONE_HOUR	60 * ONE_MINUTE
#define ONE_DAY		24 * ONE_HOUR

enum
{
	TM_YEAR	= 0x01,
	TM_MON,
	TM_DAY,
	TM_HOUR,
	TM_MIN,
	TM_SEC,
	TM_WDAY
};

#define TM_STR_FMT1	"%Y-%m-%d %H:%M:%S"
#define TM_STR_FMT2 "%Y%m%d%H%M%S"

typedef struct object_timestamp
{
	struct object parent;

	time_t seconds;
	char tbuf[BUFFER_SIZE];

	void (*_info)(void);
	int  (*_init)(object_t parent, time_t init_seconds);
	char*(*_fmt) (object_t parent, const char *fmt);
	void (*_offset) (object_t parent, int offset);
	void (*_set_by_time) (object_t parent, time_t seconds);
	void (*_set_by_str) (object_t parent, const char *fmt, const char *s);
	int  (*_get) (object_t parent, int tm_type);
}*object_timestamp_t;

object_timestamp_t new_object_timestamp(void); 

#endif
