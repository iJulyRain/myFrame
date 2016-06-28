/*
 * =====================================================================================
 *
 *       Filename:  io_pool.c
 *
 *    Description:  io pool
 *
 *        Version:  1.0
 *        Created:  06/28/2016 02:15:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (group3), lizhixian@integritytech.com.cn
 *   Organization:  
 *
 * =====================================================================================
 */

#include "common.h"
#include "io_pool.h"

#include <stdlib.h>
#include <string.h>

static void io_pool_info(void)
{
	debug(RELEASE, "==> io_pool writen by li zhixian @2016.06.28 <==\n");
}

static int io_pool_init(object_t parent)
{
    int i;
    object_io_t io = NULL;
    object_io_pool_t io_pool = NULL;

    io_pool = (object_io_pool_t)parent;

    for (i = 0; i < io_pool->pool_size; i++)
    {
        if (io_pool->mode == mode_tcp_client)
        {
            io = new_object_io_tcp("tcp client", IO_ATTR_REMOVE);
            assert(io);

            object_container_addend(&io->parent, &io_pool->container);
        }
        else if (io_pool->mode == mode_tcp_server_client)
        {
            io =  new_object_io_tcp_server_client("tcp server client");
            assert(io);

            object_container_addend(&io->parent, &io_pool->container);
        }
    }

    return 0;
}

static object_t io_pool_get_one(object_t parent)
{
    object_io_t io;
    object_io_pool_t io_pool = NULL;

    io_pool = (object_io_pool_t)parent;

    CONTAINER_FOREACH(&io_pool->container, object_io_t, io)
        if(io_state(&io->parent) == UNUSED)
            break;
        io = NULL;
    CONTAINER_FOREACH_END

    return &io->parent;
}

object_io_pool_t new_io_pool(int pool_size, int mode)
{
    object_io_pool_t io_pool = NULL;

    io_pool = (object_io_pool_t)calloc(1, sizeof(struct object_io_pool));
    assert(io_pool);

    io_pool->mode = mode;
    io_pool->pool_size = pool_size;
    object_container_init(&io_pool->container);

    io_pool->_info     = io_pool_info;
    io_pool->_init     = io_pool_init;
    io_pool->_get_one  = io_pool_get_one;

    return io_pool;
}
