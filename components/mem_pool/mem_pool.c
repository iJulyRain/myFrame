#include "mem_pool.h"

static void mem_pool_info(void)
{
	debug(RELEASE, "==> mem pool writen by li zhixian @2015.08.19 ^.^ <==\n");
}

static int mem_pool_init(object_t parent, int max_index, int max_free_index, int boundary)
{
	int i;
	object_mem_pool_t mem_pool = NULL;
	object_mem_node_t mem_node = NULL;

	mem_pool = (object_mem_pool_t)parent;

	if(max_index <= 0)
		mem_pool->max_index = MEM_INDEX_MAX;
	else
		mem_pool->max_index = max_index;

	if(max_free_index <= 0)
		mem_pool->max_free_index = MEM_INDEX_MAX_FREE;
	else
		mem_pool->max_free_index = max_free_index;

	if(boundary <= 0)
		mem_pool->boundary = MEM_BOUNDARY;
	else
		mem_pool->boundary = boundary;

	mem_pool->pool = (object_mem_node_t *)calloc(mem_pool->max_index, sizeof(object_mem_node_t));
	assert(mem_pool->pool);

	mem_pool->total = 0;	///<初始化时，不申请人和内存
	mem_pool->used = 0;

	for(i = 0; i < mem_pool->max_index; i++)
	{
		mem_node = (object_mem_node_t)calloc(1, sizeof(struct object_mem_node)); 
		assert(mem_node);

		mem_node->index = i;
		object_container_init(&mem_node->used_block_list);
		object_container_init(&mem_node->free_block_list);

		mem_pool->pool[i] = mem_node;
	}

	return 0;
}

static void *mem_pool_alloc(object_t parent, size_t size)
{
	int index;

	object_mem_pool_t mem_pool = NULL;
	object_mem_node_t mem_node = NULL;
	object_mem_block_t mem_block = NULL;

	mem_pool = (object_mem_pool_t)parent;

	///<查找与size大小最接近德node索引
	index = APR_ALIGN(size, mem_pool->boundary) / mem_pool->boundary;

	if(index >= mem_pool->max_free_index)	///<大内存，直接从OS申请
	{
		mem_block = (object_mem_block_t)calloc(1, sizeof(struct object_mem_block));
		assert(mem_block);

		mem_block->mem_node = NULL;	///<不属于任何mem_node
		mem_block->size = size;
		mem_block->avail = size;	///<全部使用

		mem_block->space = malloc(size); 
	}
	else if(index >= mem_pool->max_index)	///<大内存，但内存池可以容纳，挂接到pool[0]中
	{
		///<如果mem_node的free不为空，找一个block
		
		///<如果mem_node的free为空，创建一个block
	}
	else	///<规则内存
	{
		mem_node = mem_pool->pool[index];

		if(mem_node->free_block_list.size > 0)///<如果mem_node的free不为空，从连表头选一个block
		{
			mem_block = (object_mem_block_t)object_container_first(&mem_node->free_block_list);
			mem_block->avail = size;

			///<从free list移除
			object_container_delete((object_t)mem_block, &mem_node->free_block_list);

			///<加入used_list
			object_container_addend((object_t)mem_block, &mem_node->used_block_list);
		}
		else	///<如果mem_node的free为空，创建一个block
		{
			mem_block = (object_mem_block_t)calloc(1, sizeof(struct object_mem_block));
			assert(mem_block);

			mem_block->mem_node = mem_node;
			mem_block->size = index * mem_pool->boundary;
			mem_block->avail = size;

			mem_block->space = malloc(size); 

			///<加入used_list
			object_container_addend((object_t)mem_block, &mem_node->used_block_list);
		}
	}

	return mem_block->space;
}

static void mem_pool_free(object_t parent, void *ptr)
{

}

static void mem_pool_state(object_t parent)
{

}

object_mem_pool_t new_mem_pool(void)
{
	object_mem_pool_t mem_pool = NULL;

	mem_pool = (object_mem_pool_t)calloc(1, sizeof(struct object_mem_pool));
	assert(mem_pool);

	mem_pool->_info		= mem_pool_info;
	mem_pool->_init 	= mem_pool_init;
	mem_pool->_alloc 	= mem_pool_alloc;
	mem_pool->_free 	= mem_pool_free;
	mem_pool->_state 	= mem_pool_state;

	INIT_LOCK(&mem_pool->lock);

	return mem_pool;
}
