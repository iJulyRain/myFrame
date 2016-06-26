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
#include "thread.h"

void register_all_io(void)
{
	register_io_tcp();
	register_io_tcp_server();
	register_io_tcp_server_client();
	register_io_udp();
	register_io_com();
}

object_io_t new_object_io(const char *io_type, const char *alias, int attr)
{
	object_io_t iot, io;

	iot = (object_io_t)object_find(io_type, object_class_type_io);
	if(iot == NULL)
		return NULL;
	
	io = (object_io_t)calloc(1, sizeof(struct object_io));
	assert(io);

	*io = *iot;

	strcpy(io->parent.name, alias);
	io->user_ptr = NULL;
    io->server = NULL;
    io->attr = attr;

    if (io->attr & IO_ATTR_REMOVE)
        io->remove = TRUE;

    list_init(&io->client);
	INIT_LOCK(&io->lock);

	return io;
}

void free_object_io(object_io_t io) 
{
    if(!io)
        return;

    if(io->settings)
        free(io->settings);

    if(io->addr)
        free(io->addr);

    if(io->buffer)
        object_buffer_free(io->buffer);

    if(io->event)
        poller_event_release(io->event);

    if(io->user_ptr)
		free(io->user_ptr);

    free(io);
}

int io_getfd(object_t parent) 
{
	object_io_t io;

	io = (object_io_t)parent;

	return io->fd;
}

int io_setfd(object_t parent, int fd)
{
	object_io_t io;

	io = (object_io_t)parent;

	return (io->fd = fd);
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

	ENTER_LOCK(&io->lock);

	io->isconnect = OFFLINE;
	if (io->fd != -1)
		close(io->fd);

	EXIT_LOCK(&io->lock);

	io->fd = -1;
}

int io_recv(object_t parent)
{
	int rxnum;
	char readbuf[BUFFER_SIZE];

	object_io_t io;

	io = (object_io_t)parent;

	memset(readbuf, 0, sizeof(char) * BUFFER_SIZE);

	rxnum = read(io->fd, readbuf, BUFFER_SIZE);
	if(rxnum == 0)	///<链接断开
	{
		buffer_clear(&io->buffer->read_buf);
		return -1;
	}
	else if(rxnum == -1)	///<读出错
	{
		buffer_clear(&io->buffer->read_buf);
		return -2;
	}
	else	///<读到数据
		buffer_add(&io->buffer->read_buf, readbuf, rxnum);

	return 0;
}

int io_send(object_t parent)
{
	int txnum, size, bufsize;
	char sendbuf[BUFFER_SIZE];
	object_io_t io;
	object_thread_t ot;

	io = (object_io_t)parent;
	ot = (object_thread_t)io->hmod;

	size = buffer_size(&io->buffer->write_buf); 
	if(size > 0)
	{
		memset(sendbuf, 0, BUFFER_SIZE);
		bufsize = buffer_read(&io->buffer->write_buf, sendbuf, BUFFER_SIZE);

		txnum = write(io->fd, sendbuf, bufsize);
		if(txnum == -1 || txnum == 0)
		{
			buffer_clear(&io->buffer->write_buf);
			return -1;
		}
		else if(txnum > 0)
			buffer_remove(&io->buffer->write_buf, NULL, txnum);
	}
	else if(size == 0)	///<发送完成
	{
		poller_event_clrev(io->event, POLLOUT);	
		poller_mod((long)ot->poller, io->event);

		return 0;
	}

	return 1;
}

int io_output(object_t parent, const char *buffer, int size)
{
	object_io_t io;
	io = (object_io_t)parent;
	object_thread_t ot;
	int rc;

	ot = (object_thread_t)io->hmod;

	///<添加到缓冲区
	rc = buffer_add(&io->buffer->write_buf, buffer, size);

	///<使能写事件
	poller_event_setev(io->event, POLLOUT);	
	poller_mod((long)ot->poller, io->event);

	return rc;
}

int io_input(object_t parent, char *buffer, int size, int clear)
{
	object_io_t io;
	io = (object_io_t)parent;
	int rc;

	if(clear)
		rc = buffer_remove(&io->buffer->read_buf, buffer, size);
	else
		rc = buffer_read(&io->buffer->read_buf, buffer, size);
	
	return rc;
}
