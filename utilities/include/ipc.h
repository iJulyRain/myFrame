/*
 * =====================================================================================
 *
 *       Filename:  ipc.h
 *
 *    Description:  ipc
 *
 *        Version:  1.0
 *        Created:  2014年09月22日 16时01分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __IPC_H__
#define __IPC_H__

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>

int create_unixdomain(int type, const char *unix_file);

#endif
