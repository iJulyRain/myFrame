/*
 * =====================================================================================
 *
 *       Filename:  io_com.c
 *
 *    Description:  io com
 *
 *        Version:  1.0
 *        Created:  06/01/2015 11:43:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *        Company:  xx
 *
 * =====================================================================================
 */
#include "io.h"

struct com
{
	char dev[32];
	int baud, databit, stopbit;
	char parity;
};

static void com_info(void)
{
	debug(RELEASE, "==> AIO(com) writen by li zhixian @2015.06.01 ^.^ <==\n");
}

static int com_init(object_t parent, HMOD hmod, const char *settings)
{
	object_io_t io;
	struct com *com;

	assert(settings);

	io = (object_io_t)parent;
	io->settings = strdup(settings);
	io->hmod = hmod;
	io->mode = uart;

	debug(DEBUG, "settings: %s\n", io->settings);

	com = (struct com *)calloc(1, sizeof(struct com));
	sscanf(settings, "%[^ ] %d,%d%c%d", 
		com->dev, 
		&com->baud, &com->databit, &com->parity, &com->stopbit);
	
	io->addr = com;

	io->isconnect = OFFLINE;

	io->buffer = buffer_new();

	return 0;
}

static int com_connect(object_t parent)
{
	object_io_t io;
	struct com *com;
	const char *tty_file;

	io = (object_io_t)parent;
	com = (struct com *)io->addr;

//	tty_file = getTTY(com->dev);	///<using lua
	tty_file = com->dev;

	io->fd = open(tty_file, O_RDWR | O_NONBLOCK); 
	assert(io->fd > 0);

	io->isconnect = ONLINE;

	return io->isconnect;
}

static int com_state(object_t parent)
{
	return io_state(parent); 
}

static void com_close(object_t parent)
{
	io_close(parent);
}

static int com_recv(object_t parent)
{
	return io_recv(parent);
}

static int com_send(object_t parent)
{
	return io_send(parent);
}

static struct object_io io= 
{
	._info		= 	com_info,
	._init 		= 	com_init,
	._connect 	= 	com_connect,
	._state 	= 	com_state,
	._close 	= 	com_close,
	._recv 		= 	com_recv,
	._send 		= 	com_send
};

void register_io_com(void)
{
	object_addend(&io.parent, "io com", object_class_type_io);
}
