/*
 * =====================================================================================
 *
 *       Filename:  aio_tcp.c
 *
 *    Description:  aio tcp
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
#include "aio.h"

static void tcp_info(void)
{
	debug(RELEASE, "==> AIO(tcp) writen by li zhixian @2015.06.01 ^.^ <==")
}

static int tcp_init(object_t parent, const char *settings)
{
	object_io_t io;
	struct sockaddr_in *addr;
	char ip[16];
	int port;

	io = (object_io_t)parent;

	io->settings = strdup(settings);
	io->addr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));

	debug(DEBUG, "settings: %s\n", io->settings);

	memset(ip, 0, sizeof(ip));
	sscanf(settings, "%[^:]:%d", ip, &port);

	addr = io->addr;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(ip);
	addr->sin_port = htons(port);

	io->isconnect = OFFLINE;

	io->rbuf = buffer_new();
	io->wbuf = buffer_new();

	return 0;
}

static int tcp_connect(object_t parent)
{
	object_io_t io;
	struct sockaddr_in *addr;

	io = (object_io_t)parent;
	addr = io->addr;

	io->fd = socket(AF_INET, SOCK_STREAM, 0);
	assert(io->fd > 0);

	io->isconnect = connect(io->fd, (struct sockaddr *)io->addr, sizeof(struct sockaddr_in)) == 0 ? 1 : 0;

	return io->isconnect;
}

static int tcp_state(object_t parent)
{
	object_io_t io;

	io = (object_io_t)parent;

	return io->isconnect;
}

static void tcp_close(object_t parent)
{
	object_io_t io;

	io = (object_io_t)parent;

	close(io->fd);

	io->fd = -1;
}

static int tcp_recv(object_t parent)
{
	return 0;
}

static int tcp_send(object_t parent)
{
	return 0;
}

static struct object_io aio= 
{
	._info		= 	tcp_info,
	._init 		= 	tcp_init,
	._connect 	= 	tcp_connect,
	._state 	= 	tcp_state,
	._close 	= 	tcp_close,
	._recv 		= 	tcp_recv,
	._send 		= 	tcp_send
};

void register_aio_tcp(void)
{
	object_addend(&aio.parent, "aio tcp", object_class_type_io);
}
