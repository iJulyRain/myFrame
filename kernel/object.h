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
#include "list.h"
#include "config.h"

#include <string.h>
#include <pthread.h>

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

/**
 * @brief 对象类型
 */
enum object_class_type
{
	object_class_type_thread = 0,	///<线程对象
	object_class_type_timer,		///<定时器对象
	object_class_type_io,			///<IO对象
	object_class_type_unknown
};

/**
 * @brief 对象容器类型
 */
struct object_information
{
	enum object_class_type type;	///<类型

	pthread_mutex_t lock;		///<锁

	int size;					///<长度
	list_t list;				///<链表头
};

void global_container_init(void);
void object_container_init(struct object_information *container);
object_t object_container_first(struct object_information *container);
object_t object_container_last(struct object_information *container);
object_t object_container_find(const char *name, struct object_information *container);
object_t object_find(const char *name, int type);
void object_container_addend(object_t object, struct object_information *container);
void object_addend(object_t object, const char *name, int type);
void object_container_delete(object_t object, struct object_information *container);
void object_delete(object_t object);
void object_set_name(object_t object, const char *name);
const char *object_name(object_t object);
int object_type(object_t object);

#define CONTAINER_FOREACH(container, T, pt)	\
	list_t *node;\
	for(node = container->list.next; \
		node != &container->list; \
		node = node->next)	\
	{	\
		pt = (T)list_entry(node, struct object, list);

#define CONTAINER_FOREACH_END	}

#define OBJECT_FOREACH(type, T, pt) \
	list_t *node;\
	for(node = object_container[type].list.next; \
		node != &object_container[type].list; \
		node = node->next)	\
	{	\
		pt = (T)list_entry(node, struct object, list);

#define OBJECT_FOREACH_END	}

#endif
