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
#include <stdint.h>
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
#include <sys/wait.h>
#include <arpa/inet.h>

///<公用头文件
#include "config.h"
#include "def.h"
#include "types.h"
#include "version.h"
#include "who.h"

///<内核
#include "list.h"
#include "message.h"
#include "object.h"
#include "poller.h"
#include "print.h"
#include "thread.h"
#include "timer.h"
#include "timestamp.h"

///<组件
#include "buffer.h"
#include "io.h"
#include "database.h"
#include "thread_pool.h"
#include "mem_pool.h"

#endif
