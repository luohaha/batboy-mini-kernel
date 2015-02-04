#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#define POOL_BEGIN 0xE0000000
typedef struct block_head{
	unsigned int length : 31;//内存块的长度,包括头部长度
	unsigned int used : 1;//是否已经使用,1是已经使用，0为未使用
	struct block_head *pre;
	struct block_head *next;
}block_head;
extern block_head *leader;
void init_pool();
void* kmalloc(unsigned int length);
void kfree(unsigned int addr);
void union_block(block_head *old);
void cut(unsigned int addr,unsigned int length,block_head *prev);
void* alloc(unsigned int length,block_head *prev);
void pool_test();
#endif
