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

static void buf_base_init(buf_base_t buf)
{
	buf->size = BUFFER_SIZE; 

	buf->buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
	assert(buf->buffer);

	buf->read_pos = 0;
	buf->write_pos = 0;

	INIT_LOCK(&buf->lock);
}

///<创建一个buffer对象
object_buf_t buffer_create(void)
{
	object_buf_t ob;

	ob = (object_buf_t)calloc(1, sizeof(struct object_buf));
	assert(ob);
	
	buf_base_init(&ob->read_buf);
	buf_base_init(&ob->write_buf);

	return ob;
}

///<添加size长度的buffer数据到缓冲区中
int buffer_add(buf_base_t buf, const char *buffer, size_t size)
{
	int rc, left_size;
	buf_base_t bb;

	bb = buf; 

	ENTER_LOCK(&bb->lock);

	rc = size;

	///<剩余的长度够追加
	left_size = bb->size - bb->write_pos;
	if(left_size >= size)
	{
		memcpy(bb->buffer + bb->write_pos, buffer, size);
		bb->write_pos += size;
	}
	///<剩余的长度不够追加，但是头部空余和尾部空余之和够追加
	else if(left_size < size)
	{
		if(bb->read_pos + left_size >= size)
		{
			///<平移
			memmove(bb->buffer, bb->buffer + bb->read_pos, bb->write_pos - bb->read_pos);
			bb->write_pos = bb->write_pos - bb->read_pos; 
			bb->read_pos = 0;

			memcpy(bb->buffer + bb->write_pos, buffer, size);
			bb->write_pos += size;
		}
		///<剩余的长度不够追加，且头部空余和尾部空余之和也不够，且增加size后未超过BUFFER_MAX，平移数据&&追加
		else if(bb->read_pos + left_size < size)
		{
			if(bb->size + size <= BUFFER_MAX)
			{
				///<扩大
				bb->size += size; 
				bb->buffer = (char *)realloc(bb->buffer, bb->size);
				assert(bb->buffer);

				///<平移
				memmove(bb->buffer, bb->buffer + bb->read_pos, bb->write_pos - bb->read_pos);
				bb->write_pos = bb->write_pos - bb->read_pos; 
				bb->read_pos = 0;

				///<追加
				memcpy(bb->buffer + bb->write_pos, buffer, size);
				bb->write_pos += size;
			}
			///<剩余的长度不够追加，且头部空余和尾部空余之和也不够，且增加size后已经超过BUFFER_MAX，丢弃（极端情况，不可能无限制增加buffer）
			else if(bb->size + size > BUFFER_MAX)
			{
				///<do nothing
				rc = -1;
				debug(RELEASE, "read buffer overflow!\n");
			}
		}
	}

	EXIT_LOCK(&bb->lock);
	
	return rc;
}

///<从缓冲区中移除size长度的数据，移除的数据在buffer中
int buffer_remove(buf_base_t buf, char *buffer, size_t size)
{
	buf_base_t bb;

	bb = buf;

	ENTER_LOCK(&bb->lock);
	///<如果缓冲区没有那么多数据可读
	///<包括0缓冲区可读字节为空
	if(size > (bb->write_pos - bb->read_pos))
		size = bb->write_pos - bb->read_pos;

	if(size >  0)
	{
		if(buffer)
			memcpy(buffer, bb->buffer + bb->read_pos, size);

		bb->read_pos += size;
	}

	EXIT_LOCK(&bb->lock);

	return size;
}

///<从buffer中匹配与what相同的字节串，返回匹配到的字节串在缓冲中的起始地址
char *buffer_find(buf_base_t buf, const char *what, size_t size)
{
	int i, n, gotit;
	char *where;
	buf_base_t bb;

	bb = buf;

	ENTER_LOCK(&bb->lock);

	where = NULL;
	for(i = bb->read_pos; (i + size) <= bb->write_pos; i++)
	{
		gotit = 1;
		where = bb->buffer + i; 

		for(n = 0; n < size; n++)
		{
			if(bb->buffer[i + n] == what[n])
				continue;

			gotit = 0;
			break;
		}

		if(gotit == 1)
			break;
	}

	EXIT_LOCK(&bb->lock);

	return where;
}

///<从输入缓冲中读取size长度的数据到buffer中，输入缓冲区不改变
int buffer_read(buf_base_t buf, char *buffer, size_t size)
{
	buf_base_t bb;

	bb = buf;

	ENTER_LOCK(&bb->lock);

	///<如果缓冲区没有那么多数据可读
	///<包括0缓冲区可读字节为空
	if(size > (bb->write_pos - bb->read_pos))
		size = bb->write_pos - bb->read_pos;

	if(size > 0)
		memcpy(buffer, bb->buffer + bb->read_pos, size);

	EXIT_LOCK(&bb->lock);

	return size;
}

///<清空缓冲区
void buffer_clear(buf_base_t buf)
{
	ENTER_LOCK(&buf->lock);

	buf->read_pos = 0;
	buf->write_pos = 0;

	EXIT_LOCK(&buf->lock);
}

///<获取缓冲区数据长度
int buffer_size(buf_base_t buf)
{
	buf_base_t bb;

	bb = buf;

	return (bb->write_pos - bb->read_pos);
}
