/*
 * =====================================================================================
 *
 *       Filename:  rclient.h
 *
 *    Description:  rclient header
 *
 *        Version:  1.0
 *        Created:  2016年06月25日 15时08分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (group3), lizhixian@integritytech.com.cn
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __RCLIENT_H__
#define __RCLIENT_H__

#include "socks5.h"

#define GET_IO 0x01
#define RST_IO 0x02

struct global_conf
{
	char *server;	///<ip:port
	int ncon;
}global_conf;

int register_thread_rclient(void);

#endif
