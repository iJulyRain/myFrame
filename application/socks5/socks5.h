/*
 * =====================================================================================
 *
 *       Filename:  socks5.h
 *
 *    Description:  socks5 header file
 *
 *        Version:  1.0
 *        Created:  2016年04月03日 13时35分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (g3), lizhixian@integritytech.com.cn
 *   Organization:  g3
 *
 * =====================================================================================
 */

 #ifndef __SOCKS5_H__
 #define __SOCKS5_H__

 #define SOCK_BREAK		0x00
 #define SOCK_CONNECT	0x01

 struct s_header
 {
 	int magic;
	int command;
	char data[32];
 };

 #endif

