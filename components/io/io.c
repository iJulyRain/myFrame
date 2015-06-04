/*
 * =====================================================================================
 *
 *       Filename:  io.c
 *
 *    Description:  io
 *
 *        Version:  1.0
 *        Created:  05/26/2015 09:02:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *        Company:  xx
 *
 * =====================================================================================
 */
#include "io.h"

void register_all_io(void)
{
	register_io_tcp();
	register_io_udp();
	register_io_com();
}

int io_state(object_t parent)
{
	object_io_t io;

	io = (object_io_t)parent;

	return io->isconnect;
}

void io_close(object_t parent)
{
	object_io_t io;

	io = (object_io_t)parent;

	io->isconnect = OFFLINE;
	close(io->fd);

	io->fd = -1;
}

int io_recv(object_t parent)
{
	int rxnum;
	char readbuf[BUFFER_MAX];

	object_io_t io;

	io = (object_io_t)parent;

	memset(readbuf, 0, sizeof(char) * BUFFER_MAX);

	rxnum = read(io->fd, readbuf, BUFFER_MAX);
	if(rxnum == 0)	///<链接断开
		return -1;
	else if(rxnum == -1)	///<读出错
		return -2;
	else	///<读到数据
		buffer_add(&io->buffer->read_buf, readbuf, rxnum);

	return 0;
}

int io_send(object_t parent)
{
	int txnum, size;
	char sendbuf[BUFFER_MAX];
	object_io_t io;

	io = (object_io_t)parent;

	size = buffer_size(&io->buffer->write_buf); 
	if(size = 0)
		return 0;
	else if(size > 0)
	{
		memset(sendbuf, 0, sizeof(char) * BUFFER_MAX);
		size = buffer_read(&io->buffer->write_buf, sendbuf, size);

		txnum = write(io->fd, sendbuf, size);
		if(txnum == -1 || txnum == 0)
			return -1;
		else if(txnum > 0)
			buffer_remove(&io->buffer->write_buf, NULL, txnum);
	}

	return 0;
}
