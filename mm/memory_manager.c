#include "common.h"
#include "memory_manager.h"
#include "multiboot.h"

static unsigned int page_count=0;//目前可以供使用的内存页面的数量
static unsigned int page_stack[PAGE_NUMBER+1];//可用页面起始地址栈


void init_memory()
{
	unsigned int glb_addr   = glb_mboot_ptr->mmap_addr;
	unsigned int glb_length = glb_mboot_ptr->mmap_length;
	mmap_entry_t *addr=(mmap_entry_t *)glb_mboot_ptr->mmap_addr;
	for(;(unsigned int)addr<glb_addr+glb_length;addr++)
	{
		if(addr->type==1 && addr->base_addr_low==0x100000)
		{
			unsigned int usefull_addr=addr->base_addr_low + (unsigned int)(kern_end - kern_start);
			unsigned int max_usefull_addr=addr->base_addr_low + addr->length_low;
			for(;usefull_addr<max_usefull_addr && usefull_addr<=MAX_MEMORY_SIZE;usefull_addr+=PAGE_SIZE)
			{
				memory_free(usefull_addr);
//			printf("%d",usefull_addr);
//			printf("\n",0);
			}
		}
	}
}
void memory_free(unsigned int addr)
{
	if(page_count<PAGE_NUMBER)	
		page_stack[page_count++]=addr;
	else
		printf("ERROR: stack over flow\n",0);
}
unsigned int memory_alloc()
{
	if(page_count>0)
		return page_stack[--page_count];
	else
	{
		printf("ERROR: memory not enough\n",0);
		return -1;
	}
}

