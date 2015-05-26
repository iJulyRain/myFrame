/*
 * =====================================================================================
 *
 *       Filename:  idle.c
 *
 *    Description:	idle
 *
 *        Version:  1.0
 *        Created:  2014年09月22日 09时22分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#include "common.h"

/**
* @brief 轮询
* 这里监听来自外界的事件
*/
void idle(void)
{
	for(;;)
	{
		sleep(1);
	}
}
