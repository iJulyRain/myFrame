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
void loop(void)
{
	int i, nfds;
	struct poller_event ev[POLLER_MAX];
	object_io_t io;

	poller_id = poller_create(POLLER_MAX);
	assert(poller_id != -1);

	for(;;)
	{	
		memset(ev, 0, sizeof(struct poller_event) * POLLER_MAX);
		nfds = poller_wait(poller_id, ev, POLLER_MAX, 1000);
		if(nfds <= 0)
			continue;

		for(i = 0; i < nfds; i++)
		{
			io = (object_io_t)ev[i].ptr;

			if(ev[i].fd.revents & POLLIN)	///<可读
			{
				if(io->_recv (&io->parent) == 0)	///<读完成
					post_message(io->hmod, MSG_AIOIN, 0, (LPARAM)io);	///<有读事件
			}
			if(ev[i].fd.revents & POLLOUT)	///<可写
			{
				if(io->_send (&io->parent) == 0)	///<发送完毕
					post_message(io->hmod, MSG_AIOOUT, 0, (LPARAM)io);	///<写完成
			}
			if(ev[i].fd.revents & POLLERR)	///<写出错
			{
				post_message(io->hmod, MSG_AIOERR, 0, (LPARAM)io);
			}
			if(ev[i].fd.revents & POLLNVAL)///<描述符被关闭
			{
				post_message(io->hmod, MSG_BREAK, 0, (LPARAM)io);
			}
		}
	}
}
