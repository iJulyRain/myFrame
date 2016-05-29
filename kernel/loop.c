/*
 * =====================================================================================
 *
 *       Filename:  loop.c
 *
 *    Description:	loop	
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
	int i, rc, nfds;
	struct poller_event ev[POLLER_MAX];
	object_io_t io;

	for(;;)
	{	
		memset(ev, 0, sizeof(struct poller_event) * POLLER_MAX);
		nfds = poller_wait(0, ev, POLLER_MAX, 1000);
		if(nfds <= 0)
			continue;

		for(i = 0; i < nfds; i++)
		{
			io = (object_io_t)ev[i].ptr;

			if(ev[i].fd.revents & POLLIN)	///<可读
			{
				rc = io->_recv(&io->parent);
				if(rc == 0)	///<读到数据
					post_message(io->hmod, MSG_AIOIN, 0, (LPARAM)io);	///<有读事件
				else if(rc == -1)	///<链接断开（TCP/UDP）
				{
					poller_del(0, io->event);
					post_message(io->hmod, MSG_AIOBREAK, 0, (LPARAM)io);	///<断开
					continue;
				}
				else if(rc == -2)	///<读出错
				{
					poller_del(0, io->event);
					post_message(io->hmod, MSG_AIOERR, 0, (LPARAM)io);	///<出错
					continue;
				}
			}
			if(ev[i].fd.revents & POLLOUT)	///<可写
			{
				rc = io->_send (&io->parent); 
				if(rc == 0)	///<发送完毕
					post_message(io->hmod, MSG_AIOOUT, 0, (LPARAM)io);	///<写完成
				else if(rc == -1)	///<写出错
				{
					poller_del(0, io->event);
					post_message(io->hmod, MSG_AIOERR, 0, (LPARAM)io);	///<出错
					continue;
				}
			}
			if(ev[i].fd.revents & POLLERR)	///<出错
			{
				poller_del(0, io->event);
				post_message(io->hmod, MSG_AIOERR, 0, (LPARAM)io);
				continue;
			}
			if(ev[i].fd.revents & POLLNVAL)///<描述符被关闭
			{
				poller_del(0, io->event);
				post_message(io->hmod, MSG_AIOBREAK, 0, (LPARAM)io);
				continue;
			}
		}
	}
}
