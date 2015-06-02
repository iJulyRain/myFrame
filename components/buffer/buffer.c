/*
 * =====================================================================================
 *
 *       Filename:  buffer.c
 *
 *    Description:  buffer
 *
 *        Version:  1.0
 *        Created:  2014年12月18日 15时36分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#include "buffer.h"

object_buf_t buffer_new(void)
{
	return NULL;
}

int buffer_add(object_buf_t buf, const char *buffer, size_t size)
{
	return 0;
}

int buffer_remove(object_buf_t buf, char *buffer, size_t size)
{
	return 0;
}

char *buffer_find(object_buf_t buf, const char *what, size_t size)
{
	return NULL;
}

int buffer_setmode(object_buf_t buf, const char *head, size_t head_size, const char *tail, size_t tail_size, size_t length)
{
	return 0;
}

int buffer_read(object_buf_t buf, char *buffer, size_t size)
{
	return 0;
}

int buffer_read_pending(object_buf_t buf, char *buffer, size_t size)
{
	return 0;
}

int buffer_read_output(object_buf_t buf, char *buffer, size_t size)
{
	return 0;
}

int buffer_write(object_buf_t buf, char *buffer, size_t size)
{
	return 0;
}

