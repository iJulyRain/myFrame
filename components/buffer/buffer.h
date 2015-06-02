#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "object.h"
#include <stddef.h>

enum buf_mode
{
	head_tail_length = 1,
	head_tail,
	head_length,
	tail,
	unknown
};

/**
* @brief 缓冲类
*/
typedef struct object_buf
{
	struct object parent;

	int buf_size;
	char *buffer;
	int read_pos, write_pos;

	//以下成员被设置后
	//buffer可以根据设置的包特点
	//匹配并返回一个完整的包
	int buf_mode;
	char *head, *tail;	///<头、尾特征(string or HEX)
	size_t head_size, tail_size;	///<头、尾特征长度
	size_t length;	///<包长度

	char *output;	///<匹配到的包
	size_t output_size;	///<匹配到的包长度
}*object_buf_t;	

object_buf_t buffer_new(void);
int buffer_add(object_buf_t buf, const char *buffer, size_t size);
int buffer_remove(object_buf_t buf, char *buffer, size_t size);
char *buffer_find(object_buf_t buf, const char *what, size_t size);
int buffer_setmode(object_buf_t buf, const char *head, size_t head_size, const char *tail, size_t tail_size, size_t length);
int buffer_read(object_buf_t buf, char *buffer, size_t size);
int buffer_read_pending(object_buf_t buf, char *buffer, size_t size);
int buffer_read_output(object_buf_t buf, char *buffer, size_t size);
int buffer_write(object_buf_t buf, char *buffer, size_t size);

#endif
