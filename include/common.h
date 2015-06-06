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

///<公告头文件
#include "def.h"
#include "print.h"
#include "version.h"
#include "who.h"

///<功能组件
#include "timer.h"
#include "message.h"
#include "poller.h"
#include "io.h"

struct object_information object_container[object_class_type_unknown];

////////////////////////////////////////////////////////////////////////
int init(int argc, char **argv);
void app_init(int argc, char **argv);

void loop(void);

int register_thread_daemon(void);

#endif
