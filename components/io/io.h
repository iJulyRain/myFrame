#ifndef __AIO_H__
#define __AIO_H__

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "def.h"
#include "object.h"
#include "buffer.h"
#include "print.h"

/**
* @brief IO接口对象
* 目前支持
* 1、串口
* 2、TCP client、UDP client、unixdomain client
*/
typedef struct object_io
{
	struct object parent;	///<基类
	HMOD hmod;

	int fd;
	int isconnect;

	//配置信息(拷贝) 字符串型
	//串口：COM1 9600,8n1
	//TCP client：192.168.1.100:10001
	//UDP client：192.168.1.100:10001
	//unixdomain client: /tmp/myframe.socket
	const char *settings;

	void *addr;	///<网络IPC的地址信息

	object_buf_t rbuf, wbuf;	///<读写缓存

	void (*_info)	(void);	///<接口版本信息
	int  (*_init)	(object_t parent, const char* settings);	///<初始化
	int  (*_connect)(object_t parent);	///<连接
	int  (*_state)	(object_t parent);	///<状态
	void (*_close)	(object_t parent);	///<关闭
	int  (*_recv)	(object_t parent);	///<读
	int  (*_send)	(object_t parent);	///<写
}*object_io_t;

void register_all_io(void);

void register_io_tcp(void);
void register_io_udp(void);
void register_io_com(void);

#endif
