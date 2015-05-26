/*
 * =====================================================================================
 *
 *       Filename:  serial.h
 *
 *    Description:  serialport
 *
 *        Version:  1.0
 *        Created:  2014年09月22日 16时08分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <termios.h>

#include "print.h"

const char *get_dev(int port);
int open_serial_port(int port, int baud, int databits, int stopbits, int parity);

#endif
