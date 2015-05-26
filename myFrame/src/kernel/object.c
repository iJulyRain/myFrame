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
#include "common.h"

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

/**
* @brief 对象迭代器
*
* 这个函数不是安全的，当有其他线程操作对量链表时
* 有可能造成不可预知的后果
*
* @param type 对象类型，索引对象容器
* @param po 当前对象位置
*
* @return 遍历完返回NULL，否则返回下一个对象指针 
*/
object_t object_iter(int type, object_t po)
{
	list_t *node;
	object_t p;
	struct object_information *information;

	if(type < 0 || type > object_class_type_unknown)
		return NULL;

	information = &object_container[type];
	
	if(po == NULL)
		node = information->list.next;
	else
		node = po->list.next;

	if(node == &information->list)
		return NULL;

	p = list_entry(node, struct object, list);

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
    int find_obj = 0;
	list_t *node;
	object_t p = NULL;
	struct object_information *information;

	if(type < 0 || type > object_class_type_unknown)
		return NULL;

	information = &object_container[type];

	ENTER_LOCK(&object_container[type].lock);

	for(node = information->list.next; node != &information->list; node = node->next)
	{
		p = list_entry(node, struct object, list);
		if(!strcmp(p->name, name))
        {
            find_obj = 1;
            break;
        }
	}

	EXIT_LOCK(&object_container[type].lock);

    if(find_obj == 0)
        return NULL;

	return p;
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
	list_t *list;

	if(type < 0 || type > object_class_type_unknown)
		return;
	
	strncpy(object->name, name, OBJ_NAME_MAX);
	object->type = type;
	
	list = &object_container[type].list;

	ENTER_LOCK(&object_container[type].lock);

	list_insert_before(list, &object->list);

	EXIT_LOCK(&object_container[type].lock);

	object_container[type].size ++;
}

/**
* @brief 删除对象
*
* @param object 对象
*/
void object_delete(object_t object)
{
	ENTER_LOCK(&object_container[object->type].lock);

	list_remove(&object->list);

	EXIT_LOCK(&object_container[object->type].lock);
}
