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
#include "loop.h"

void do_loop(long pfd, int timeout)
{
	int i, rc, nfds;
	struct poller_event ev[POLLER_MAX];
	object_io_t io;

	memset(ev, 0, sizeof(struct poller_event) * POLLER_MAX);
	nfds = poller_wait(pfd, ev, POLLER_MAX, timeout);
	if(nfds <= 0)
		return;

	for(i = 0; i < nfds; i++)
	{
		io = (object_io_t)ev[i].ptr;

        if(io_state(&io->parent) != ONLINE) ///<在外部被关闭
            continue;

		if(ev[i].fd.revents & POLLIN)	///<可读
		{
			rc = io->_recv(&io->parent);
			if(rc == 0)	///<读到数据
				send_message(io->hmod, MSG_AIOIN, 0, (LPARAM)io);	///<有读事件
			else if(rc == -1)	///<链接断开（TCP/UDP）
            {
				send_message(io->hmod, MSG_AIOBREAK, 0, (LPARAM)io);	///<断开
                continue;
            }
			else if(rc == -2)	///<读出错
            {
				send_message(io->hmod, MSG_AIOERR, 0, (LPARAM)io);	///<出错
                continue;
            }
		}

		if(ev[i].fd.revents & POLLOUT)	///<可写
		{
			rc = io->_send (&io->parent); 
			if(rc == 0)	///<发送完毕
				send_message(io->hmod, MSG_AIOOUT, 0, (LPARAM)io);	///<写完成
			else if(rc == -1)	///<写出错
            {
				send_message(io->hmod, MSG_AIOERR, 0, (LPARAM)io);	///<出错
                continue;
            }
		}

		if(ev[i].fd.revents & POLLERR)	///<出错
        {
			send_message(io->hmod, MSG_AIOERR, 0, (LPARAM)io);
            continue;
        }

		if(ev[i].fd.revents & POLLNVAL)///<描述符被关闭
        {
			send_message(io->hmod, MSG_AIOBREAK, 0, (LPARAM)io);
            continue;
        }
	}

	for(i = 0; i < nfds; i++)
    {
		io = (object_io_t)ev[i].ptr;

        if(io_state(&io->parent) != ONLINE)
        {
			send_message(io->hmod, MSG_AIOBREAK, 0, (LPARAM)io);	///<清除

            if(io->remove)
			    send_message(io->hmod, MSG_AIOCLR, 0, (LPARAM)io);	///<清除
        }
    }
}

/**
* @brief 轮询
* 这里监听来自外界的事件
*/
void loop(void)
{
	for(;;)
		do_loop(0, 1000);
}
