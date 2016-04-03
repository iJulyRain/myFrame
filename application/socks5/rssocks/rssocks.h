/*
 * =====================================================================================
 *
 *       Filename:  rssocks.h
 *
 *    Description:  rcsocks header file
 *
 *        Version:  1.0
 *        Created:  2016年04月02日 19时37分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (g3), lizhixian@integritytech.com.cn
 *   Organization:  g3
 *
 * =====================================================================================
 */
#ifndef __RCSOCKS_H__
#define __RCSOCKS_H__

#include "socks5.h"

#define GET_IO 0x01
#define RST_IO 0x02

int register_thread_rssocks_a(void);
int register_thread_rssocks_b(void);

struct global_conf
{
	char *server;	///<ip:port
	int ncon;
}global_conf;

#endif
