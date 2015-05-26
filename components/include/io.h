/*
 * =====================================================================================
 *
 *       Filename:  io.h
 *
 *    Description:  io 
 *
 *        Version:  1.0
 *        Created:  2014年09月22日 17时04分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __IO_H__
#define __IO_H__

int buffer_readbuf(object_t parent, char *buffer, int size);
int buffer_sendbuf(object_t parent, const char *buffer, const int size);
int buffer_get_pending(object_buf_t buf, char *buffer, int size);
int buffer_get_buffer(object_buf_t buf, char *buffer, int size);

void register_uart_io_operations(void);
void register_tcp_io_operations(void);
void register_udp_io_operations(void);

#endif
