/*
 * =====================================================================================
 *
 *       Filename:  uart.c
 *
 *    Description:  uart
 *
 *        Version:  1.0
 *        Created:  2014年08月28日 15时16分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#include "common.h"

/**
* @brief 串口 接口集信息 
*/
static void uart_info(void)
{
	debug(DEBUG, "===> IO using uart writen by li zhixian @ July 2, 2014 <===\n");
}

/**
* @brief 串口 初始化
*
* @param parent 需要初始化的对象
* @param info 初始化参数
*
* @return 成功返回0，失败返回-1
*/
static int uart_init(object_t parent, void *info, object_buf_t read_buf, object_buf_t send_buf)
{
	struct uart_setting *s;
	object_io_t oi;

	s = (struct uart_setting *)info;

	oi = (object_io_t)parent;

	oi->cfg = (struct uart_setting *)calloc(1, sizeof(struct uart_setting));
	assert(oi->cfg);

	*(struct uart_setting *)oi->cfg = *s;

	debug(DEBUG, "===> uart '%d' %d%c%d%d\n",
		s->which,
		s->baud, s->parity, s->databit, s->stopbit);

	oi->connect = OFFLINE;

	if(read_buf)
	{
		oi->read_buf = (object_buf_t)calloc(1, sizeof(struct object_buf));
		assert(oi->read_buf);

		oi->read_buf->read_pos = oi->read_buf->write_pos = 0;
		oi->read_buf->read_cb = read_buf->read_cb;
		oi->read_buf->send_cb = read_buf->send_cb;
		oi->read_buf->buf_size = read_buf->buf_size;
		oi->read_buf->buffer = (char *)calloc(oi->read_buf->buf_size, sizeof(char));
		assert(oi->read_buf->buffer);
	}

	if(send_buf)
	{
		oi->send_buf = (object_buf_t)calloc(1, sizeof(struct object_buf));
		assert(oi->send_buf);

		oi->send_buf->read_pos = oi->send_buf->write_pos = 0;
		oi->send_buf->read_cb = send_buf->read_cb;
		oi->send_buf->send_cb = send_buf->send_cb;
		oi->send_buf->buf_size = send_buf->buf_size;
		oi->send_buf->buffer = (char *)calloc(oi->send_buf->buf_size, sizeof(char));
		assert(oi->send_buf->buffer);
	}

	return 0;
}

/**
* @brief 串口 关闭
*
* @param parent 需要关闭的对象
*/
static void uart_close(object_t parent)
{
	object_io_t oi = (object_io_t)parent;

	oi->connect = OFFLINE;
	close(oi->fd);
}

static int uart_poll(object_t parent, int timeout)
{
	struct pollfd fds[1];
	object_io_t oi;
	
	oi = (object_io_t)parent;

	memset(fds, 1, sizeof(struct pollfd) * 1);
	fds[0].fd = oi->fd; 
	fds[0].events = POLLIN;

	return poll(fds, 1, timeout);
}

/**
* @brief 串口连接
*
* @param parent 发起连接的对象
*
* @return 总是返回连接成功
*/
static int uart_connect(object_t parent)
{
	object_io_t oi;
	struct uart_setting *s;

	oi = (object_io_t)parent;

	s = oi->cfg;

	oi->fd = open_serial_port(s->which, s->baud, s->databit, s->stopbit, s->parity);
	if(oi->fd < 0)
	{
		oi->connect = OFFLINE;
		return -1;
	}
	else
		oi->connect = ONLINE;
	
	return oi->connect;
}

static int uart_state(object_t parent)
{
	object_io_t oi;
	
	oi = (object_io_t)parent;

	return oi->connect;
}

/**
* @brief 串口 接收
*
* @param parent 接收数据的对象
* @param buffer 接收数据缓冲区
* @param size 接收数据长度
*
* @return 成功返回读取的长度，失败返回-1
*/
static int uart_recv(object_t parent, char *buffer, int size)
{
	object_io_t oi = (object_io_t)parent;;

	return read(oi->fd, buffer, size);
}

/**
* @brief 串口 发送
*
* @param parent 发送数据的对象
* @param buffer 发送的数据
* @param size 发送的数据长度
*
* @return 成功返回发送的长度，失败返回-1 
*/
static int uart_send(object_t parent, const char *buffer, const int size)
{
	object_io_t oi = (object_io_t)parent;

	return write(oi->fd, buffer, size);
}

int uart_readbuf(object_t parent, char *buffer, int size)
{
	return buffer_readbuf(parent, buffer, size); 
}

int uart_sendbuf(object_t parent, const char *buffer, const int size)
{
	return buffer_sendbuf(parent, buffer, size); 
}

/**
* @brief 注册 串口 IO 操作接口集
*/
void register_uart_io_operations(void)
{
	object_io_t oi;

	oi = (object_io_t)calloc(1, sizeof(struct object_io));
	assert(oi);
	
	oi->_info	= uart_info;
	oi->_init	= uart_init;
	oi->_connect= uart_connect;
	oi->_state  = uart_state;
	oi->_close	= uart_close;
	oi->_poll   = uart_poll;
	oi->_recv	= uart_recv;
	oi->_send	= uart_send;
	oi->_readbuf  = uart_readbuf;
	oi->_sendbuf  = uart_sendbuf;

	object_addend(&oi->parent, "uart", object_class_type_io);
}
