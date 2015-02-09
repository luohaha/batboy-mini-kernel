#include "common.h"
#include "memory_manager.h"
#include "virtual_memory.h"
#include "memory_pool.h"
#include "monitor.h"
//用来标记按页块分配的当前最大地址,起始是POOL_BEGIN
static unsigned int pool_top_page; 
//标记内存池当前已经分配到的位置,起始是POOL_BEGIN
static unsigned int pool_top;
//内存块链表头部，不使用
block_head *leader;
void init_pool()
{
	pool_top_page=POOL_BEGIN;
	pool_top=POOL_BEGIN;
	//新建一个长度为PAGE_SIZE的内存块
	//不试用，只是作为链表的头部，大方吧
	leader=(block_head *)pool_top;
	unsigned int newpage=memory_alloc();
	map(pde_kern,(unsigned int)leader,newpage,PAGE_P|PAGE_RW);
	leader->length=sizeof(block_head);
	leader->used=1;
	leader->pre=0;
	leader->next=0;
	pool_top_page=POOL_BEGIN+PAGE_SIZE;
	pool_top=POOL_BEGIN+leader->length;
}
void* kmalloc(unsigned int length)
//申请内存函数,返回可用空间的首地址
{
	block_head *point=leader;//查找可用块的指针
	block_head *tmppre=point;//用来暂存point的上一个
	while(point!=0)
	{
		//if(point->used==0&&point->length>=sizeof(block_head)*2+length)
		if(point->used==0&&point->length>=length+sizeof(block_head))
		//如果能找到一个合适内存块		
		{
			if(point->length>=sizeof(block_head)*2+length)
			{
				point->used=1;
				unsigned int tmp=point->length;
				point->length=length+sizeof(block_head);
				//告诉cut函数，被截断后新的起点和长度,还有它的前面的block地址
				cut((unsigned int)point+point->length,tmp-point->length,point);
				return (void *)((unsigned int)point+sizeof(block_head));
			}
			else
			{
				point->used=1;
				return (void *)((unsigned int)point+sizeof(block_head));
			}
		}
		//如果不能，就继续往下找
		tmppre=point;
		point=point->next;
	}
	//如果没有现成的内存块，那要自己申请一个
	return alloc(length+sizeof(block_head),tmppre);
}
void kfree(unsigned int addr)
//释放可用地址addr的内存块
{
	block_head *old =(block_head *)(addr-sizeof(block_head));
	old->used=0;
	union_block(old);
}
void union_block(block_head *old)
{
	if(old->next!=0&&old->next->used==0)
	//可以和后边的未使用的合并
	{
		old->length=old->length+old->next->length;
		if(old->next->next!=0)
		{
			old->next->next->pre=old;
		}
		old->next=old->next->next;
	}
	if(old->pre!=0&&old->pre->used==0)
	//和前面未使用的合并
	{
		old->pre->length=old->pre->length+old->length;
		if(old->next)
		{
			old->next->pre=old->pre;
		}
		old->pre->next=old->next;
		old=old->pre;
	}
	if(old->next==0)
	//回收物理内存
	{
		unsigned int tmplength=old->length;
		old->pre->next=0;
		while(pool_top-tmplength+PAGE_SIZE<=pool_top_page)
		{
			pool_top_page-=PAGE_SIZE;
			unsigned int page;
			get_mapping(pde_kern,pool_top_page,&page);//获取对应的物理地址
			unmap(pde_kern,pool_top_page);//解除映射
			memory_free(page);//回收物理内存
		}
		pool_top-=tmplength;
	}
}
void cut(unsigned int addr,unsigned int length,block_head *prev)
//从地址addr开始，截下包括头部的长度length,前面的是prev
{
	block_head *cutnew=(block_head *)addr;
	cutnew->used=0;
	cutnew->length=length;
	cutnew->pre=prev;
	cutnew->next=prev->next;
	prev->next->pre=cutnew;
	prev->next=cutnew;
}
void* alloc(unsigned int length,block_head *prev)
//这里的length包括头部
{
	if(pool_top+length<=pool_top_page)
	//当前分配的物理地址已足够使用
	{
		block_head *new=(block_head *)pool_top;
		new->used=1;
		new->length=length;
		new->next=0;
		new->pre=prev;
		prev->next=new;
		pool_top+=length;
		return (void *)((unsigned int)pool_top-length+sizeof(block_head));
		
	}
	else
	//需要再次分配物理内存
	{
		while(pool_top+length>pool_top_page)
		{
			unsigned int newpage=memory_alloc();
			map(pde_kern,pool_top_page,newpage,PAGE_P|PAGE_RW);
			pool_top_page+=PAGE_SIZE;

		}
		block_head *new=(block_head *)pool_top;
		new->used=1;
		new->length=length;
		new->next=0;
		new->pre=prev;
		prev->next=new;
		pool_top+=length;
		return (void *)((unsigned int)pool_top-length+sizeof(block_head));
	}
} 
void pool_test()
{
	void *addr=kmalloc(50);
	printf("\nkalloc 50B, addr: ",0);
	printf("%h",(unsigned int)addr);
	void *addr2=kmalloc(50);
	printf("\nkalloc 50B, addr: ",0);
	printf("%h",(unsigned int)addr2);
	void *addr4=kmalloc(50);
	printf("\nkalloc 50B, addr: ",0);
	printf("%h",(unsigned int)addr4);
	kfree((unsigned int)addr);
	printf("\nkfree 50B, addr: ",0);
	printf("%h",(unsigned int)addr);
	
	kfree((unsigned int)addr2);
	printf("\nkfree 50B, addr: ",0);
	printf("%h",(unsigned int)addr2);
	void *addr3=kmalloc(70);
	printf("\nkalloc 70B, addr: ",0);
	printf("%h",(unsigned int)addr3);
	kfree((unsigned int)addr3);
	printf("\nkfree 70B, addr: ",0);
	printf("%h",(unsigned int)addr3);
}
