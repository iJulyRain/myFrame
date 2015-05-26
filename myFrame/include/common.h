/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  common
 *
 *        Version:  1.0
 *        Created:  2014年09月19日 15时33分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <termios.h>
#include <semaphore.h>
#include <errno.h> 
#include <poll.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <arpa/inet.h>

#include "def.h"
#include "list.h"
#include "object.h"
#include "print.h"
#include "types.h"
#include "message.h"
#include "timer.h"
#include "config.h"

#include "version.h"
#include "who.h"

#include "io.h"

#include "sem.h"
#include "tools.h"
#include "ipc.h"
#include "serial_port.h"

#include <sqlite3.h>

struct uart_setting
{
	int which;

	int baud;
	int databit;
	int stopbit; 
	int parity;
};

struct net_setting
{
	char ip[16];
	int port;
};


struct object_information object_container[object_class_type_unknown];

////////////////////////////////////////////////////////////////////////
int init(void);
void idle(void);

int register_thread_daemon(void);

#endif
