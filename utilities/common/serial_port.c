/*
 * =====================================================================================
 *
 *       Filename:  serial_port.c
 *
 *    Description:  serial_port
 *
 *        Version:  1.0
 *        Created:  2013年11月27日 11时31分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#include "serial_port.h"

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

	if(tcgetattr(fd, &buf)<0)
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
	if((buf.c_lflag & (ECHO | ICANON | IEXTEN | ISIG)) || (buf.c_iflag & (BRKINT | ICRNL | INPCK | ISTRIP | IXON )) || (buf.c_cflag & ( CSIZE | PARENB | CS8)) != CS8 || (buf.c_oflag & OPOST) || buf.c_cc[VMIN]!=1 || buf.c_cc[VTIME]!=0)
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
static int set_Parity(int fd,int databits,int stopbits,int parity)
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

/**
* @brief PLATFORM 801 tty files 
*/
static const char *_801_tty_file[] = 
{
	"/dev/ttyS0", 
	"/dev/ttyS2", 
	"/dev/ttyS1", 
	"/dev/ttySC4", 
	"/dev/ttySC5", 
	"/dev/ttySC2", 
	"/dev/ttySC3", 
	"/dev/ttySC0", 
	"/dev/ttySC1",
	"/dev/ttyS3",
};

/**
* @brief PLATFORM 802 tty files
*/
static const char *_802_tty_file[] = 
{
	"/dev/ttyS0", 
	"/dev/ttyS1", 
	"/dev/ttyS2", 
	"/dev/ttySC4", 
	"/dev/ttySC5", 
	"/dev/ttySC2", 
	"/dev/ttySC3", 
	"/dev/ttySC0", 
	"/dev/ttySC1",
	"/dev/ttyS3",
};

/**
* @brief PLATFORM 704 tty files
*/
static const char *_704_tty_file[] = 
{
	"/dev/ttyS0", 
	"/dev/ttyS1", 
	"/dev/ttyS2", 
	"/dev/ttySA4", 
	"/dev/ttySA5", 
	"/dev/ttySA6", 
	"/dev/ttySA7", 
	"/dev/ttySA0", 
	"/dev/ttySA1", 
	"/dev/ttySA2", 
	"/dev/ttySA3",
};

const char *get_dev(int port)
{
	char *p;
	int platform;
	const char **device_file = NULL;

	p = getenv("PLATFORM");
	if(p == NULL)
	{
		debug(RELEASE, "unknown platform!\n");
		return NULL;
	}

	platform = atoi(p); 

	switch (platform)
	{
		case 801:
			device_file = _801_tty_file; 
			break;
		case 802:
			device_file = _802_tty_file; 
			break;
		case 704:
			device_file = _704_tty_file; 
			break;
	}

	return device_file[port];
}

/**
* @brief user's interface, open serial port and init
*
* @param port port number, begin with 0
* @param baud baud
* @param databits databits
* @param stopbits stopbits
* @param parity parity 'o'/'e'/'n'
*
* @return success return fd, failed return -1 
*/
int open_serial_port(int port, int baud, int databits, int stopbits, int parity)
{
	int fd;
	const char *dev;

	dev = get_dev(port);

	fd = open(dev, O_RDWR | O_NONBLOCK);
	if(fd < 0)
		return -1;
	
	tty_raw(fd);
	set_speed(fd, baud);
	set_Parity(fd, databits, stopbits, parity);

	return fd;
}
