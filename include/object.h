/*
 * =====================================================================================
 *
 *       Filename:  object.h
 *
 *    Description:  object
 *
 *        Version:  1.0
 *        Created:  2014年09月19日 15时35分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "def.h"

void object_container_init(void);
object_t object_iter(int type, object_t po);
object_t object_find(const char *name, int type);
void object_addend(object_t object, const char *name, int type);
void object_delete(object_t object);

#endif
