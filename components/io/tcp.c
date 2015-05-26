/*
 * =====================================================================================
 *
 *       Filename:  tcp.c
 *
 *    Description:  tcp
 *
 *        Version:  1.0
 *        Created:  2014年08月28日 15时16分47秒
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
* @brief TCP 接口集信息
*/
static void tcp_info(void)
{
	debug(DEBUG, "===> IO using tcp writen by li zhixian @ june 12, 2014 <===\n");
}

/**
* @brief TCP 初始化
*
* @param parent 需要初始化的对象
* @param info 初始化的参数
*
* @return 成功返回9，失败返回-1 
*/
static int tcp_init(object_t parent, void *info, object_buf_t read_buf, object_buf_t send_buf)
{
	struct net_setting *s;
	struct sockaddr_in *saddr;

	object_io_t oi = (object_io_t)parent;
	s = (struct net_setting *)info;

	oi->cfg = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));
	assert(oi->cfg);

	saddr = oi->cfg;

	saddr->sin_family = AF_INET;
	saddr->sin_addr.s_addr = inet_addr(s->ip);
	saddr->sin_port = htons(s->port);

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
* @brief TCP 关闭
*
* @param parent 需要关闭的对象
*/
static void tcp_close(object_t parent)
{
	object_io_t oi = (object_io_t)parent;

	oi->connect = OFFLINE;
	close(oi->fd);
}

static int tcp_poll(object_t parent, int timeout)
{
	struct pollfd fds[1];
	object_io_t oi = (object_io_t)parent;

	memset(fds, 0, sizeof(struct pollfd) * 1);
	fds[0].fd = oi->fd; 
	fds[0].events = POLLIN;

	return poll(fds, 1, timeout);
}

/**
* @brief TCP 连接
*
* @param parent 发起连接的对象
*
* @return 连接成功返回1，连接失败返回0 
*/
static int tcp_connect(object_t parent)
{
	object_io_t oi;
	struct sockaddr_in *saddr;
	
	oi = (object_io_t)parent;
	saddr = oi->cfg;

	oi->fd = socket(AF_INET, SOCK_STREAM, 0);
	if(oi->fd < 0)
	{
		debug(RELEASE, "==> create socket error!\n");
		return -1;
	}

    return (oi->connect = (connect(oi->fd, (struct sockaddr *)saddr, sizeof(struct sockaddr_in)) == 0) ? 1 : 0);
}

static int tcp_state(object_t parent)
{
    object_io_t oi = (object_io_t)parent;

    return oi->connect;
}

/**
* @brief TCP 接收
*
* @param parent 接收数据的对象 
* @param buffer 接收数据缓冲区
* @param size 接收数据长度
*
* @return 成功返回接收长度，失败返回-1 
*/
static int tcp_recv(object_t parent, char *buffer, int size)
{	
	int rxnum;
	object_io_t oi;
	struct sockaddr_in *saddr;
	
	oi = (object_io_t)parent;
	saddr = oi->cfg;

	rxnum = recv(oi->fd, buffer, size, 0);
	if(rxnum <= 0)
	{
		tcp_close(parent);

		debug(RELEASE, "'%s' connect to server %s@%d break!\n", 
			parent->name,
			inet_ntoa(saddr->sin_addr),
			ntohs(saddr->sin_port));
	}

	return rxnum;
}

/**
* @brief TCP 发送
*
* @param parent 发送数据的对象
* @param buffer 发送的数据
* @param size 发送的数据长度
*
* @return 成功返回发送长度，失败返回-1
*/
static int tcp_send(object_t parent, const char *buffer, const int size)
{
	int txnum;
	object_io_t oi;
	struct sockaddr_in *saddr;

	oi = (object_io_t)parent;
	saddr = oi->cfg;

	txnum = send(oi->fd, buffer, size, 0);
	if(txnum <= 0)
	{
		tcp_close(parent);

		debug(RELEASE, "'%s' connect to server %s@%d break!\n", 
			parent->name,
			inet_ntoa(saddr->sin_addr),
			ntohs(saddr->sin_port));
	}

	return txnum;
}

int tcp_readbuf(object_t parent, char *buffer, int size)
{
	return buffer_readbuf(parent, buffer, size); 
}

int tcp_sendbuf(object_t parent, const char *buffer, const int size)
{
	return buffer_sendbuf(parent, buffer, size); 
}

/**
* @brief 注册 TCP IO 操作接口集
*/
void register_tcp_io_operations(void)
{
	object_io_t oi;

	oi = (object_io_t)calloc(1, sizeof(struct object_io));
	assert(oi);
	
	oi->_info	       = tcp_info;
	oi->_init	       = tcp_init;
	oi->_connect       = tcp_connect;
    oi->_state		   = tcp_state;
	oi->_close	       = tcp_close;
	oi->_poll          = tcp_poll;
	oi->_recv	       = tcp_recv;
	oi->_send	       = tcp_send;
	oi->_readbuf	   = tcp_readbuf;
	oi->_sendbuf	   = tcp_sendbuf;

	object_addend(&oi->parent, "tcp", object_class_type_io);
}
