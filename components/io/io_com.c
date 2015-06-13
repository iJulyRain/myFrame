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

#include <termios.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>

#define NAME "io com"

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

static int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300};
static int name_arr[] = {115200, 38400, 19200, 9600, 4800, 2400, 1200, 300};

/**
 * @brief 设置端口模式
 *
 * @param fd 串口文件描述符
 *
 * @return 成功返回0，失败返回-1
 */
static int tty_raw(int fd)
{
	int err;
	struct termios buf;
	struct termios save_termios;

	if(tcgetattr(fd, &buf) < 0)
		return -1;
	save_termios = buf;

	buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	buf.c_cflag &= ~(CSIZE | PARENB);
	buf.c_cflag |= CS8;
	buf.c_oflag &= ~(OPOST);

	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	if(tcsetattr(fd, TCSAFLUSH, &buf)<0)
		return -1;

	if(tcgetattr(fd, &buf)<0)
	{
		err = errno;
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = err;
		return -1;
	}
	if((buf.c_lflag & (ECHO | ICANON | IEXTEN | ISIG)) 
	|| (buf.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON )) 
	|| (buf.c_cflag & ( CSIZE | PARENB | CS8)) != CS8 
	|| (buf.c_oflag & OPOST) 
	|| buf.c_cc[VMIN] != 1 
	|| buf.c_cc[VTIME] != 0)
	{
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = EINVAL;
		return -1;
	}

	return 0;
}

/**
 * @brief 设置终端波特率
 *
 * @param fd	串口文件描述符
 * @param speed	波特率
 *
 * @return 成功返回0，失败发回-1
 */
static int set_speed(int fd, int speed)
{
	int	i;
	int	status;
	struct termios	Opt;
	tcgetattr(fd, &Opt);
	for(i = 0; i < sizeof(speed_arr) / sizeof(int); i++) 
	{
		if(speed == name_arr[i])
		{
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if(status != 0) 
			{
				perror("tcsetattr fd");
				return -1;
			}
			tcflush(fd,TCIOFLUSH);
		}
	}
	return 0;
}


/**
 * @brief 设置终端属性
 *
 * @param fd 串口文件描述符
 * @param databits 数据位
 * @param stopbits 停止位
 * @param parity 校验位
 *
 * @return 成功返回0，失败返回-1
 */
static int set_parity(int fd,int databits,int stopbits,int parity)
{
	struct termios options;
	if  ( tcgetattr( fd,&options)  !=  0) {
		perror("SetupSerial 1");
		return 1;
	}
	options.c_cflag &= ~CSIZE;
	switch (databits)
	{
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			break;
	}
	switch (parity)
	{
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */
			break;
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB);
			options.c_iflag |= INPCK;             /* Disnable parity checking */
			break;
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;     /* Enable parity */
			options.c_cflag &= ~PARODD;
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 'S':
		case 's':  /*as no parity*/
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;break;
		default:
			return 1;
	}
	switch (stopbits)
	{
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
			break;
		default:
			return 1;
	}
	/* Set input parity option */
	if (parity != 'n')
		options.c_iflag |= INPCK;
	tcflush(fd,TCIFLUSH);
	options.c_cc[VTIME] = 150;
	options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
	if (tcsetattr(fd,TCSANOW,&options) != 0)
	{
		perror("SetupSerial 3");
		return 1;
	}

	return 0;
}

static int com_init(object_t parent, HMOD hmod, const char *settings)
{
	object_io_t io;
	struct com *com;

	assert(settings);

	io = (object_io_t)parent;
	io->settings = strdup(settings);
	io->hmod = hmod;
	io->mode = mode_uart;

	debug(DEBUG, "settings: %s\n", io->settings);

	com = (struct com *)calloc(1, sizeof(struct com));
	sscanf(settings, "%[^ ] %d,%d%c%d", 
		com->dev, 
		&com->baud, &com->databit, &com->parity, &com->stopbit);
	
	io->addr = com;

	io->isconnect = OFFLINE;

	io->buffer = buffer_create();
	io->event = poller_event_create(io);

	return 0;
}

static int com_connect(object_t parent)
{
	object_io_t io;
	struct com *com;

	io = (object_io_t)parent;
	com = (struct com *)io->addr;

	io->fd = open(com->dev, O_RDWR | O_NONBLOCK); 
	assert(io->fd > 0);

	tty_raw(io->fd);
	set_speed(io->fd, com->baud);
	set_parity(io->fd, com->databit, com->stopbit, com->parity);

	io->isconnect = ONLINE;

	return io->isconnect;
}

static int com_getfd(object_t parent)
{
	return io_getfd(parent);
}

static int com_setfd(object_t parent, int fd)
{
	return io_setfd(parent, fd);
}

static int com_state(object_t parent)
{
	return io_state(parent); 
}

static void com_close(object_t parent)
{
	io_close(parent);
}

static int com_output(object_t parent, const char *buffer, int size)
{
	return io_output(parent, buffer, size); 
}

static int com_input(object_t parent, char *buffer, int size, int clear)
{
	return io_input(parent, buffer, size, clear); 
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
	._getfd		=	com_getfd,
	._setfd		=	com_setfd,
	._state 	= 	com_state,
	._close 	= 	com_close,
	._input		=	com_input,
	._output	= 	com_output,
	._recv 		= 	com_recv,
	._send 		= 	com_send
};

void register_io_com(void)
{
	object_addend(&io.parent, NAME, object_class_type_io);
}

object_io_t new_object_io_com(const char *alias)
{
	return new_object_io(NAME, alias);
}
