#ifndef __AIO_H__
#define __AIO_H__

#include "object.h"
#include "message.h"
#include "poller.h"
#include "print.h"
#include "config.h"

#include "buffer.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>

enum
{
	mode_tcp_client = 1,
	mode_tcp_server,
	mode_tcp_server_client,
	mode_udp_client,
	mode_udp_server,
	mode_unixdomain_client,
	mode_unixdomain_server,
	mode_uart
};

#define IO_ATTR_REMOVE 0x0001 ///<remove if close

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
	int isconnect;	///<0 OFFLINE / 1 ONLINE / 2 CONNECTIONG
	int mode;	///<模式

    int attr;
    int remove; ///<remove from container

    struct object_io *server; ///< only apply to tcp_server_client mode

	list_t client; ///<server accept's clients, only apply to tcp_server mode

	pthread_mutex_t lock;

	//配置信息(拷贝) 字符串型
	//串口：COM1 9600,8n1
	//TCP client/UDP client/TCP server/UDP server：192.168.1.100:10001
	//unixdomain client: /tmp/myframe.socket
	char *settings;

	void *addr;	///<网络IPC的地址信息

	object_buf_t buffer;	///<读写缓存
	poller_event_t event;	///<托管到poller的event

	void (*_info)	(void);	///<接口版本信息
	int  (*_init)	(object_t parent, HMOD hmod, const char* settings);	///<初始化
	int  (*_connect)(object_t parent);	///<连接
	int  (*_getfd)	(object_t parent);	///<描述符
	int  (*_setfd)	(object_t parent, int fd);	///<设置描述符
	int  (*_state)	(object_t parent);	///<状态
	void (*_close)	(object_t parent);	///<关闭
	int  (*_input)	(object_t parent, char *buffer, int size, int clear);	///<读
	int  (*_output)	(object_t parent, const char *buffer, int size);	///<写

	int  (*_recv)	(object_t parent);	///<读缓冲
	int  (*_send)	(object_t parent);	///<写缓冲

	void *user_ptr;
}*object_io_t;

object_io_t new_object_io(const char *io_type, const char *alias, int attr);
void free_object_io(object_io_t io);

int io_getfd(object_t parent);
int io_setfd(object_t parent, int fd);
int io_state(object_t parent);
void io_close(object_t parent);
int io_output(object_t parent, const char *buffer, int size);
int io_input(object_t parent, char *buffer, int size, int clear);
int io_recv(object_t parent);
int io_send(object_t parent);

void register_io_tcp(void);
void register_io_tcp_server(void);
void register_io_tcp_server_client(void);
void register_io_udp(void);
void register_io_com(void);

void register_all_io(void);

object_io_t new_object_io_tcp_server(const char *alias);
object_io_t new_object_io_tcp_server_client(const char *alias);
object_io_t new_object_io_tcp(const char *alias, int attr);
object_io_t new_object_io_udp(const char *alias, int attr);
object_io_t new_object_io_com(const char *alias, int attr);

#endif
