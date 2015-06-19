/*
 * =====================================================================================
 *
 *       Filename:  object.c
 *
 *    Description:  object
 *
 *        Version:  1.0
 *        Created:  2014年05月07日 11时52分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (R&D), lzx1442@163.com
 *        Company:  wanwei-tech
 *
 * =====================================================================================
 */
#include "object.h"

extern struct object_information object_container[object_class_type_unknown];

/**
* @brief 初始化对象容器
*/
void object_container_init(void)
{
	int i;

	for(i = 0; i < object_class_type_unknown; i++)
	{
		object_container[i].type = i;

		///<指向自己
		object_container[i].list.prev = &object_container[i].list;
		object_container[i].list.next = &object_container[i].list;

		INIT_LOCK(&object_container[i].lock);
	}
}

object_t object_container_find(const char *name, struct object_information *container)
{
    int find_obj = 0;
	list_t *node;
	object_t p = NULL;

	ENTER_LOCK(&container->lock);

	for(node = container->list.next; node != &container->list; node = node->next)
	{
		p = list_entry(node, struct object, list);
		if(!strcmp(p->name, name))
        {
            find_obj = 1;
            break;
        }
	}

	EXIT_LOCK(&container->lock);

    if(find_obj == 0)
        return NULL;

	return p;
}

/**
* @brief 查找对象
*
* @param name 对象名称
* @param type 对象类型
*
* @return 查找成功返回对象地址，失败返回NULL 
*/
object_t object_find(const char *name, int type)
{
	struct object_information *information;

	if(type < 0 || type > object_class_type_unknown)
		return NULL;

	information = &object_container[type];

	return object_container_find(name, information);
}

void object_container_addend(object_t object, struct object_information *container)
{
	list_t *list;

	list = &container->list;

	ENTER_LOCK(&container->lock);

	list_insert_before(list, &object->list);

	EXIT_LOCK(&container->lock);

	container->size ++;
}

/**
* @brief 添加对象
*
* @param object 对象
* @param name 对象名称
* @param type 对象类型
*/
void object_addend(object_t object, const char *name, int type)
{
	struct object_information *container;

	if(type < 0 || type > object_class_type_unknown)
		return;
	
	strncpy(object->name, name, OBJ_NAME_MAX);
	object->type = type;

	container = object_container + type;

	object_container_addend(object, container);
}

void object_container_delete(object_t object, struct object_information *container)
{
	ENTER_LOCK(&container->lock);

	list_remove(&object->list);

	EXIT_LOCK(&container->lock);
}

/**
* @brief 删除对象
*
* @param object 对象
*/
void object_delete(object_t object)
{
	struct object_information *container;

	container = object_container + object->type; 

	object_container_delete(object, container);
}

const char *object_name(object_t object)
{
	return object->name;
}

int object_type(object_t object)
{
	return object->type;
}