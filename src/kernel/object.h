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

#include "list.h"
#include "config.h"

/**
 * @brief 对象基类
 */
typedef struct object
{
	char name[OBJ_NAME_MAX];	///<基类名称
	int type;		///<对象类型
	int flag;		///<对象标志

	list_t list;	///<节点
}*object_t;

void object_container_init(void);
object_t object_find(const char *name, int type);
void object_addend(object_t object, const char *name, int type);
void object_delete(object_t object);

#define OBJECT_FOREACH(type, T, pt) \
	list_t *node;\
	for(node = object_container[type].list.next; \
		node != &object_container[type].list; \
		node = node->next)	\
	{	\
		pt = (T)list_entry(node, struct object, list);

#define OBJECT_FOREACH_END	}

#endif
