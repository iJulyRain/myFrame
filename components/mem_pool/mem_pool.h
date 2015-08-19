#ifndef __MEM_POOL_H__
#define __MEM_POOL_H__ 

#include "object.h"
#include "print.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MEM_INDEX_MAX		20	///<内存池最大规则内存 4096 * 20 = 80K Byte
#define MEM_INDEX_MAX_FREE	40	///<内存池最大容纳内存 4096 * 40 = 160K Byte
#define MEM_BOUNDARY 		4096	///<4K Byte 

///<内存池模型
//	[mem_pool]
//	|
//	|_____pool [0][1][2][3][4][5]....[MEM_INDEX_MAX-1][MEM_INDEX_MAX]
//				|  |
//				|  |	
//				|  |___[mem_node]
//				|      [  used  ]=[mem_block]=[mem_block]=[mem_block]
//				|      [  free  ]=[mem_block]=[mem_block]
//				|
//				|___[mem_node]
//					[  used  ]=[mem_block]=[mem_block]=[mem_block]
//					[  free  ]=[mem_block]=[mem_block]

///<内存节点结构
typedef struct object_mem_node
{
	struct object parent;

	int index;	///<mem_node在pool的index

	struct object_information used_block_list;	///<已用block列表
	struct object_information free_block_list;	///<空闲block列表
}*object_mem_node_t;

///<内存块结构
typedef struct object_mem_block
{
	struct object parent;

	object_mem_node_t mem_node;	///<mem_block所属的mem_node

	size_t size;	///<mem_block的大小
	size_t avail; 	///<mem_block真实使用的大小（统计碎片）

	void *space;	///<OS内存
}*object_mem_block_t;

///内存池结构
typedef struct object_mem_pool
{
	struct object parent;

	///<内存池可以容纳的最大空间(4096 * max_index byte)
	int max_index;

	///<内存池可以容纳德最大非规则空间 (4096 * max_free_index)
	///<当用户层申请的空间大于max_free_index，在释放空间的时候
	///<内存池直接将空间释放给OS，否则挂接到pool[0]侠
	int max_free_index;

	size_t total;	///<内存池目前总共管理的内存大小
	size_t used;	///<目前在用的内存大小

	int boundary;

	pthread_mutex_t lock;

	object_mem_node_t *pool;	///<内存池	

	void (*_info)(void);
	int  (*_init)(object_t parent, int max_index, int max_free_index, int boundary);
	void* (*_alloc) (object_t parent, size_t size);
	void (*_free)  (object_t parent, void *ptr);
	void (*_state) (object_t parent);
}*object_mem_pool_t;

#define APR_ALIGN(size, boundary) \
	(((size)+ ((boundary) - 1)) &~((boundary) - 1))

object_mem_pool_t new_mem_pool(void);

#endif
