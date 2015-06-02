/*
 * =====================================================================================
 *
 *       Filename:  io.c
 *
 *    Description:  io
 *
 *        Version:  1.0
 *        Created:  05/26/2015 09:02:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *        Company:  xx
 *
 * =====================================================================================
 */
#include "io.h"

void register_all_io(void)
{
	register_io_tcp();
	register_io_udp();
	register_io_com();
}
