/*
 * =====================================================================================
 *
 *       Filename:  io_tcp_server_client_client.c
 *
 *    Description:  io tcp server cleint
 *
 *        Version:  1.0
 *        Created:  06/13/2015 10:26:51 PM
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

#define NAME "io tcp server client"

static void tcp_server_client_info(void)
{
	debug(RELEASE, "==> AIO(tcp server client) writen by li zhixian @2015.06.13 ^.^ <==\n");
}

static int tcp_server_client_init(object_t parent, HMOD hmod, const char *settings)
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
	io->mode = mode_tcp_server_client;

	debug(DEBUG, "settings: %s\n", io->settings);

	io->addr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));

	memset(ip, 0, sizeof(ip));
	sscanf(settings, "%[^:]:%d", ip, &port);

	addr = io->addr;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(ip);
	addr->sin_port = htons(port);

	io->isconnect = ONLINE;

	io->buffer = object_buffer_create();
	io->event = poller_event_create(io);

	object_container_addend(&io->parent, &this->io_container);	///<填充到线程的IO容器里面

	return 0;
}

static int tcp_server_client_connect(object_t parent)
{
	object_io_t io = (object_io_t)parent;

	return (io->isconnect = ONLINE);
}

static int tcp_server_client_getfd(object_t parent)
{
	return io_getfd(parent); 
}

static int tcp_server_client_setfd(object_t parent, int fd)
{
	return io_setfd(parent, fd);
}

static int tcp_server_client_state(object_t parent)
{
	return io_state(parent); 
}

static void tcp_server_client_close(object_t parent)
{
	io_close(parent);
}

static int tcp_server_client_output(object_t parent, const char *buffer, int size)
{
	return io_output(parent, buffer, size); 
}

static int tcp_server_client_input(object_t parent, char *buffer, int size, int clear)
{
	return io_input(parent, buffer, size, clear); 
}

static int tcp_server_client_recv(object_t parent)
{
	return io_recv(parent);
}

static int tcp_server_client_send(object_t parent)
{
	return io_send(parent);
}

static struct object_io io= 
{
	._info		= 	tcp_server_client_info,
	._init 		= 	tcp_server_client_init,
	._connect 	= 	tcp_server_client_connect,
	._getfd 	= 	tcp_server_client_getfd,
	._setfd		= 	tcp_server_client_setfd,
	._state 	= 	tcp_server_client_state,
	._close 	= 	tcp_server_client_close,
	._input		=	tcp_server_client_input,
	._output	= 	tcp_server_client_output,
	._recv 		= 	tcp_server_client_recv,
	._send 		= 	tcp_server_client_send
};

void register_io_tcp_server_client(void)
{
	object_addend(&io.parent, NAME, object_class_type_io);
}

object_io_t new_object_io_tcp_server_client(const char *alias)
{
	return new_object_io(NAME, alias, IO_ATTR_REMOVE);
}
