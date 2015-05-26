/*
 * =====================================================================================
 *
 *       Filename:  ipc.c
 *
 *    Description:  ipc
 *
 *        Version:  1.0
 *        Created:  2013年11月26日 14时18分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#include "ipc.h"
#include "print.h"

/**
* @brief 创建unix domain套接字
*
* @param type 套接字类型，SOCK_STREAM/SOCK_DGRAM
* @param unix_file 套接字文件路径
*
* @return 成功返回描述符，失败返回-1
*/
int create_unixdomain(int type, const char *unix_file)
{
	if(unix_file == NULL)
		return -1;
		
	struct sockaddr_un un;
	int sd = 0, ret = 0;
	socklen_t len = 0;
	
	sd = socket(AF_UNIX, type, 0);
	if(sd < 0)
	{
		debug(RELEASE, "failed to create socket(unix domain)!!!\n");
		return -1;
	}
	
	fcntl(sd, F_SETFL, O_NONBLOCK | fcntl(sd, F_GETFL));
	memset(&un, 0, sizeof(struct sockaddr_un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, unix_file);
	unlink(unix_file);
	
	len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
	ret = bind(sd, (struct sockaddr *)&un, len);
	
	if(ret < 0)
	{
		debug(RELEASE, "failed to bind(%s)!!!\n", unix_file);
		return -1;
	}
	
	return sd;
}
