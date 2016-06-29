/*
 * =====================================================================================
 *
 *       Filename:  io_tcp_server_server.c
 *
 *    Description:  io tcp server
 *
 *        Version:  1.0
 *        Created:  06/13/2015 09:59:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  julyrain (RD), lzx1442@163.com
 *        Company:  xx
 *
 * =====================================================================================
 */
#include "io.h"
#include "thread.h"

#define NAME "io tcp server"

static void tcp_server_info(void)
{
	debug(RELEASE, "==> AIO(tcp server) writen by li zhixian @2015.06.13 ^.^ <==\n");
}

static int tcp_server_init(object_t parent, HMOD hmod, const char *settings)
{
	object_io_t server;
	struct sockaddr_in *addr;
	char ip[16];
	int rc, port, client_max, backlog, option = 1;
	object_thread_t this = (object_thread_t)hmod;

	assert(settings);

	server = (object_io_t)parent;

	server->settings = strdup(settings);
	debug(DEBUG, "settings: %s\n", server->settings);

	server->hmod = hmod;
	server->mode = mode_tcp_server;

	server->addr = (struct sockaddr_in *)calloc(1, sizeof(struct sockaddr_in));

	memset(ip, 0, sizeof(ip));
	sscanf(settings, "%[^:]:%d:%d:%d", ip, &port, &backlog, &client_max);

	addr = server->addr;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr(ip);
	addr->sin_port = htons(port);

	server->fd = socket(AF_INET, SOCK_STREAM, 0);
	assert(server->fd > 0);

	setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, (void *)&option, sizeof(option));

	rc = bind(server->fd, (struct sockaddr *)addr, sizeof(struct sockaddr_in));
    if (rc < 0)
    {
        debug(ERROR, "==> bind port %d failed!\n", port);
        return -1;
    }

	rc = listen(server->fd, backlog);
    if (rc < 0)
    {
        debug(ERROR, "==> listen failed\n");
        return -1;
    }

	server->isconnect = OFFLINE;

	server->buffer = NULL;	///<监听socket不需要buffer
	server->event = poller_event_create(server);

    server->io_pool = new_io_pool(object_name(&server->parent), client_max, mode_tcp_server_client);
    assert(server->io_pool);

    server->io_pool->_init(&server->io_pool->parent);

	object_container_addend(&server->parent, &this->io_container);	///<填充到线程的IO容器里面

	return 0;
}

static int tcp_server_connect(object_t parent)
{
	object_io_t io = (object_io_t)parent;

	return (io->isconnect = ONLINE);
}

static int tcp_server_getfd(object_t parent)
{
	return io_getfd(parent); 
}

static int tcp_server_setfd(object_t parent, int fd)
{
	return io_setfd(parent, fd); 
}

static int tcp_server_state(object_t parent)
{
	return io_state(parent); 
}

static void tcp_server_close(object_t parent)
{
	io_close(parent);
}

static int tcp_server_output(object_t parent, const char *buffer, int size)
{
	return 0; 
}

static int tcp_server_input(object_t parent, char *buffer, int size, int clear)
{
	return 0; 
}

static int tcp_server_recv(object_t parent)
{
	int sd, port;
	char ip[16], settings[32];
	struct sockaddr_in addr;
	socklen_t size;
	object_io_t server, client;
	object_thread_t this;
	
	server = (object_io_t)parent;
	this = (object_thread_t)server->hmod;

	memset(&addr, 0, sizeof(struct sockaddr_in));
	size = sizeof(struct sockaddr_in);

	sd = accept(server->fd, (struct sockaddr *)&addr, &size);
	if(sd < 0)
		return -2;
	
	memset(ip, 0, sizeof(ip));
	strncpy(ip, inet_ntoa(addr.sin_addr), 15);
	port = ntohs(addr.sin_port);

	memset(settings, 0, sizeof(settings));
	snprintf(settings, 31, "%s:%d", ip, port);

	///<创建一个对等端
    /*
	client = new_object_io_tcp_server_client(settings);
	client->_setfd(&client->parent, sd);
	client->_init(&client->parent, server->hmod, settings); ///<与监听描述符同一个线程
    */

    client = (object_io_t)server->io_pool->_get_one(&server->io_pool->parent);
    if (!client)
    {
        debug(ERROR, "==> <%s> io pool use up!\n", object_name(&server->io_pool->parent));
        close(sd);

        return 0;
    }

	client->_setfd(&client->parent, sd);
	client->_init(&client->parent, server->hmod, settings);

	debug(DEBUG, "==> new client: '%s' connected\n", settings);

    client->server = server;

    ///<added to poller
	poller_event_setfd(client->event, sd);
	poller_add((long)this->poller,  client->event);

	send_message(server->hmod, MSG_AIOCONN, 0, (LPARAM)client);

	return 0; 
}

static int tcp_server_send(object_t parent)
{
	return 0; 
}

static struct object_io io= 
{
	._info		= 	tcp_server_info,
	._init 		= 	tcp_server_init,
	._connect 	= 	tcp_server_connect,
	._getfd 	= 	tcp_server_getfd,
	._setfd 	= 	tcp_server_setfd,
	._state 	= 	tcp_server_state,
	._close 	= 	tcp_server_close,
	._input		=	tcp_server_input,
	._output	= 	tcp_server_output,
	._recv 		= 	tcp_server_recv,
	._send 		= 	tcp_server_send
};

void register_io_tcp_server(void)
{
	object_addend(&io.parent, NAME, object_class_type_io);
}

object_io_t new_object_io_tcp_server(const char *alias)
{
	return new_object_io(NAME, alias, 0);
}
