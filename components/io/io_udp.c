/*
 * =====================================================================================
 *
 *       Filename:  io_udp.c
 *
 *    Description:  io udp
 *
 *        Version:  1.0
 *        Created:  06/01/2015 11:43:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *        Company:  xx
 *
 * =====================================================================================
 */
#include "io.h"

static void udp_info(void)
{
	debug(RELEASE, "==> IO(udp) writen by li zhixian @2015.06.01 ^.^ <==")
}

static int udp_init(object_t parent, const char *settings)
{
	object_io_t io;
	struct sockaddr_in *addr;
	char ip[16];
	int port;

	assert(settings);

	io = (object_io_t)parent;
	io->settings = strdup(settings);

	debug(DEBUG, "settings: %s\n", io->settings);

	io->addr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));

	memset(ip, 0, sizeof(ip));
	sscanf(settings, "%[^:]:%d", ip, &port);

	addr = io->addr;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(ip);
	addr->sin_port = htons(port);

	io->isconnect = OFFLINE;

	io->buffer = buffer_new();

	return 0;
}

static int udp_connect(object_t parent)
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

static int udp_state(object_t parent)
{
	return io_state(parent); 
}

static void udp_close(object_t parent)
{
	io_close(parent);
}

static int udp_recv(object_t parent)
{
	return io_recv(parent);
}

static int udp_send(object_t parent)
{
	return io_send(parent);
}

static struct object_io io= 
{
	._info		= 	udp_info,
	._init 		= 	udp_init,
	._connect 	= 	udp_connect,
	._state 	= 	udp_state,
	._close 	= 	udp_close,
	._recv 		= 	udp_recv,
	._send 		= 	udp_send
};

void register_io_udp(void)
{
	object_addend(&io.parent, "io udp", object_class_type_io);
}
