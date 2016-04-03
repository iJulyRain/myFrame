/*
 * =====================================================================================
 *
 *       Filename:  rcsocks.h
 *
 *    Description:  rcsocks header file
 *
 *        Version:  1.0
 *        Created:  2016年04月02日 14时09分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (g3), lizhixian@integritytech.com.cn
 *   Organization:  g3
 *
 * =====================================================================================
 */

/*--------------------------------------------------------------------------------------

([target LAN]<--->[rssocks])<---WAN--->[rcsocks_b|rcsocks_a]<---WAN--->[proxychains]

--------------------------------------------------------------------------------------*/

#ifndef __RCSOCKS_H__
#define __RCSOCKS_H__

#include "socks5.h"

enum socks_state
{
	socks_state_version = 0,
	socks_state_connect,
	socks_state_stream
};

#define GET_IO	0x01
#define RST_IO	0x02

struct control_block
{
	int state;

	object_io_t bind_io;
};

struct global_conf
{
	int listen_port;
	int reverse_port;
}global_conf;

int register_thread_rcsocks_a(void);
int register_thread_rcsocks_b(void);

#endif
