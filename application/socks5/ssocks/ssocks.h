/*
 * =====================================================================================
 *
 *       Filename:  ssocks.h
 *
 *    Description:  ssocks header
 *
 *        Version:  1.0
 *        Created:  2016年05月28日 23时13分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (g3), lizhixian@integritytech.com.cn
 *   Organization:  g3
 *
 * =====================================================================================
 */
#ifndef __SOCKS_H__
#define __SOCKS_H__

#include "common.h"

enum socks_state
{
	socks_state_version = 0,
	socks_state_connect,
	socks_state_stream
};

struct cb
{
	int state;
	object_io_t bind;
};

struct global_conf
{
	int listen_port; ///< SOCKS listen port
	int ncon; ///< max connect
}global_conf;

int register_thread_ssocks(void);

#endif
