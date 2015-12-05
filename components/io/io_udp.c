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
#include "thread.h"

#define NAME	"io udp"

static void udp_info(void)
{
	debug(RELEASE, "==> AIO(udp) writen by li zhixian @2015.06.01 ^.^ <==\n");
}

static int udp_init(object_t parent, HMOD hmod, const char *settings)
{
	object_io_t io;
	struct sockaddr_in *addr;
	char ip[16];
	int port;
	object_thread_t this = (object_thread_t)hmod;

	assert(settings);

	io = (object_io_t)parent;
	io->settings = strdup(settings);
	io->hmod = hmod;
	io->mode = mode_udp_client;

	debug(DEBUG, "settings: %s\n", io->settings);

	io->addr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));

	memset(ip, 0, sizeof(ip));
	sscanf(settings, "%[^:]:%d", ip, &port);

	addr = io->addr;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(ip);
	addr->sin_port = htons(port);

	io->isconnect = OFFLINE;

	io->buffer = object_buffer_create();
	io->event = poller_event_create(io);

	object_container_addend(&io->parent, &this->io_container);	///<填充到线程的IO容器里面

	return 0;
}

static int udp_connect(object_t parent)
{
	object_io_t io;

	io = (object_io_t)parent;

	io->fd = socket(AF_INET, SOCK_DGRAM, 0);
	assert(io->fd > 0);

	io->isconnect = (connect(io->fd, (struct sockaddr *)io->addr, sizeof(struct sockaddr_in)) == 0) ? ONLINE : OFFLINE;

	return io->isconnect;
}

static int udp_getfd(object_t parent)
{
	return io_getfd(parent);
}

static int udp_setfd(object_t parent, int fd)
{
	return io_setfd(parent, fd);
}

static int udp_state(object_t parent)
{
	return io_state(parent); 
}

static void udp_close(object_t parent)
{
	io_close(parent);
}

static int udp_output(object_t parent, const char *buffer, int size)
{
	return io_output(parent, buffer, size); 
}

static int udp_input(object_t parent, char *buffer, int size, int clear)
{
	return io_input(parent, buffer, size, clear); 
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
	._getfd		= 	udp_getfd,
	._setfd		= 	udp_setfd,
	._state 	= 	udp_state,
	._close 	= 	udp_close,
	._input		=	udp_input,
	._output	= 	udp_output,
	._recv 		= 	udp_recv,
	._send 		= 	udp_send
};

void register_io_udp(void)
{
	object_addend(&io.parent, NAME, object_class_type_io);
}

object_io_t new_object_io_udp(const char *alias)
{
	return new_object_io(NAME, alias);
}
