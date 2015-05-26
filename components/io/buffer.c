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
#include "common.h"

int buffer_readbuf(object_t parent, char *buffer, int size)
{
	int i, rxnum;
	object_io_t oi;
	object_buf_t buf;

	oi = (object_io_t)parent;

	buf = oi->read_buf;

	if(buf == NULL)	///<未使用buf，此接口不使能
	{
		debug(RELEASE, "(readbuf) '%s' not used buf!\n", parent->name);
		return 0;
	}

	///<读数据
	rxnum = oi->_recv(parent, buffer, size);
	if(rxnum <= 0)
	{
		debug(RELEASE, "(readbuf) '%s' connect to server break!\n", parent->name);

		return -1;
	}

	///<将读取的数据
	for(i = 0; i < rxnum; i++)
	{
		if((buf->write_pos + 1) % buf->buf_size == buf->read_pos)
		{
			debug(RELEASE, "(readbuf) '%s' buffer full!\n", parent->name);
			buf->read_pos = buf->write_pos = 0;	///<reset buffer

			break;
		}
			
		buf->buffer[buf->write_pos] = buffer[i];
		buf->write_pos = (buf->write_pos + 1) % buf->buf_size;	///<更新写指针
	}

	///<调用读回调
	if(buf->read_cb)
		buf->read_cb(parent);

	return i;
}

int buffer_sendbuf(object_t parent, const char *buffer, const int size)
{
	int i;
	object_io_t oi;
	object_buf_t buf;

	oi = (object_io_t)parent;
	buf = oi->send_buf;

	if(buf == NULL)	///<未使用buf，此接口不使能
	{
		debug(RELEASE, "(sendbuf) '%s' not used buf!\n", parent->name);
		return 0;
	}

	for(i = 0; i < size; i ++)
	{
		if((buf->write_pos + 1) % buf->buf_size == buf->read_pos);
		{
			debug(RELEASE, "(sendbuf) '%s' buffer full!\n", parent->name);
			break;
		}
			
		buf->buffer[buf->write_pos] = buffer[i];
		buf->write_pos = (buf->write_pos + 1) % buf->buf_size;	///<更新写指针
	}

	if(buf->send_cb)
		buf->send_cb(parent);

	return i;
}

/**
* @brief 获取buf中未处理的数据
* 不会改变buf结构中的read_pos指针
*
* @param buf buffer结构
* @param buffer 读取缓冲
* @param size 读取缓冲长度
*
* @return buf中未处理的数据长度
*/
int buffer_get_pending(object_buf_t buf, char *buffer, int size)
{
	int i, read_pos;

	for(i = 0; i < size; i++)
	{
		read_pos = (buf->read_pos + i) % buf->buf_size;
		if(read_pos == buf->write_pos)	///<读完了
			break;

		buffer[i] = buf->buffer[read_pos];
	}

	return i;
}

/**
* @brief 获取buf中的数据
* 会改变buf结构中的read_pos指针
*
* @param buf buffer结构
* @param buffer 读取缓冲
* @param size 读取缓冲长度
*
* @return 读取到的buf数据长度 
*/
int buffer_get_buffer(object_buf_t buf, char *buffer, int size) 
{
	int i, read_pos;

	read_pos = buf->read_pos;

	for(i = 0; i < size; i++)
	{
		if(read_pos == buf->write_pos)	///<读完了
			break;

		buffer[i] = buf->buffer[read_pos];

		read_pos = (read_pos + 1) % buf->buf_size;
	}

	buf->read_pos = read_pos;

	return i;
}
