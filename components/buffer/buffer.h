#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "object.h"
#include "print.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

typedef struct buf_base
{
	int size;		///<缓冲区长度
	char *buffer;	///<缓冲区
	int read_pos, write_pos;	///<读写偏移

	pthread_mutex_t lock;
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

////////////////////////////////////////////////////////////////////
object_buf_t object_buffer_create(void);
void object_buffer_free(object_buf_t buf);

int buffer_add(buf_base_t buf, const char *buffer, size_t size);
int buffer_remove(buf_base_t buf, char *buffer, size_t size);
char *buffer_find(buf_base_t buf, const char *what, size_t size);
int buffer_read(buf_base_t buf, char *buffer, size_t size);
void buffer_clear(buf_base_t buf);
int buffer_size(buf_base_t buf);

#endif
