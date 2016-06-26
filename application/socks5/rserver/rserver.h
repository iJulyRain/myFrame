/*
 * =====================================================================================
 *
 *       Filename:  rserver.h
 *
 *    Description:  rserver header
 *
 *        Version:  1.0
 *        Created:  2016年06月23日 15时01分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (group3), lizhixian@integritytech.com.cn
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __RSERVER_H__
#define __RSERVER_H__

#include "socks5.h"

enum socks_state
{
	socks_state_version = 0,
	socks_state_connect,
	socks_state_stream
};

struct control_block
{
	int state;

	object_io_t io_bind;
};

#define GET_IO	0x01
#define RST_IO	0x02

struct global_conf
{
	int listen_port;
	int reverse_port;
}global_conf;

int register_thread_rserver(void);

#endif
