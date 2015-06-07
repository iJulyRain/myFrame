/*
 * =====================================================================================
 *
 *       Filename:  io_tcp.c
 *
 *    Description:  io tcp
 *
 *        Version:  1.0
 *        Created:  06/01/2015 11:42:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *        Company:  xx
 *
 * =====================================================================================
 */
#include "io.h"

static void tcp_info(void)
{
	debug(RELEASE, "==> AIO(tcp) writen by li zhixian @2015.06.01 ^.^ <==\n");
}

static int tcp_init(object_t parent, HMOD hmod, const char *settings)
{
	object_io_t io;
	struct sockaddr_in *addr;
	char ip[16];
	int port;

	assert(settings);

	io = (object_io_t)parent;
	io->settings = strdup(settings);
	io->hmod = hmod;
	io->mode = mode_tcp_client;

	debug(DEBUG, "settings: %s\n", io->settings);

	io->addr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));

	memset(ip, 0, sizeof(ip));
	sscanf(settings, "%[^:]:%d", ip, &port);

	addr = io->addr;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(ip);
	addr->sin_port = htons(port);

	io->isconnect = OFFLINE;

	io->buffer = buffer_create();
	io->event = poller_event_create(io);

	return 0;
}

static int tcp_connect(object_t parent)
{
	object_io_t io;
	struct sockaddr_in *addr;
	int rc, conn;
	int e;
	socklen_t elen;

	io = (object_io_t)parent;
	addr = io->addr;

	if(io->isconnect == OFFLINE)
	{
		io->fd = socket(AF_INET, SOCK_STREAM, 0);
		assert(io->fd > 0);
		fcntl(io->fd, F_SETFL, fcntl(io->fd, F_GETFL) | O_NONBLOCK);

		conn = connect(io->fd, (struct sockaddr *)io->addr, sizeof(struct sockaddr_in));
		if(conn == -1)
		{
			if(errno == EINPROGRESS)	///<说明还在继续
				io->isconnect = CONNECTING;
			else
				io->isconnect = OFFLINE;	///<出错
		}
		else if(conn == 0)
			io->isconnect = ONLINE;
	}
	else if(io->isconnect == CONNECTING)
	{
		elen = sizeof(e);
		rc = getsockopt(io->fd, SOL_SOCKET, SO_ERROR, (void *)&e, &elen);
		if(rc < 0)
			io->isconnect = OFFLINE;

		if(e)
		{
			if(e == EWOULDBLOCK 
			|| e == EINPROGRESS
			|| e == EINTR
			|| e == EINVAL)	///<还没连上
				io->isconnect = CONNECTING;
			else
				io->isconnect = OFFLINE;
		}

		io->isconnect = ONLINE;
	}

	return io->isconnect;
}

static int tcp_getfd(object_t parent)
{
	return io_getfd(parent); 
}

static int tcp_state(object_t parent)
{
	return io_state(parent); 
}

static void tcp_close(object_t parent)
{
	io_close(parent);
}

static int tcp_output(object_t parent, const char *buffer, int size)
{
	return io_output(parent, buffer, size); 
}

static int tcp_input(object_t parent, char *buffer, int size, int clear)
{
	return io_input(parent, buffer, size, clear); 
}

static int tcp_recv(object_t parent)
{
	return io_recv(parent);
}

static int tcp_send(object_t parent)
{
	return io_send(parent);
}

static struct object_io io= 
{
	._info		= 	tcp_info,
	._init 		= 	tcp_init,
	._connect 	= 	tcp_connect,
	._getfd 	= 	tcp_getfd,
	._state 	= 	tcp_state,
	._close 	= 	tcp_close,
	._input		=	tcp_input,
	._output	= 	tcp_output,
	._recv 		= 	tcp_recv,
	._send 		= 	tcp_send
};

void register_io_tcp(void)
{
	object_addend(&io.parent, "io tcp", object_class_type_io);
}
