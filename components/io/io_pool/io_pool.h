/*
 * =====================================================================================
 *
 *       Filename:  io_pool.h
 *
 *    Description:  io pool
 *
 *        Version:  1.0
 *        Created:  06/28/2016 02:15:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (group3), lizhixian@integritytech.com.cn
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __IO_POOL_H__
#define __IO_POOL_H__

#include "object.h"

typedef struct object_io_pool
{
    struct object parent;

    int mode;

    int pool_size;
    struct object_information container;

    void (*_info) (void);
    int  (*_init) (object_t parent);

    object_t (*_get_one) (object_t parent);
}*object_io_pool_t;

object_io_pool_t new_io_pool(const char *alias, int pool_size, int mode);

#endif
