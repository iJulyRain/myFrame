#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#include "print.h"
#include "object.h"

typedef struct buf_base
{
	int size;
	char *buffer;
	int read_pos, write_pos;
}*buf_base_t;

/**
* @brief 缓冲类
*/
typedef struct object_buf
{
	struct object parent;

	struct buf_base read_buf;
	struct buf_base write_buf;
}*object_buf_t;	

object_buf_t buffer_new(void);
int buffer_add(object_buf_t buf, const char *buffer, size_t size);
int buffer_remove(object_buf_t buf, char *buffer, size_t size);
char *buffer_find(object_buf_t buf, const char *what, size_t size);
int buffer_read(object_buf_t buf, char *buffer, size_t size);
void buffer_clear(buf_base_t buf);

#endif
