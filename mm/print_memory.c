#include "memory_manager.h"
#include "monitor.h"

void print_memory()
{
	//先找到mmap_entry的地址
	mmap_entry_t *mmap=(mmap_entry_t*)glb_mboot_ptr->mmap_addr;
	//地址分布的长度
	unsigned int length=glb_mboot_ptr->mmap_length;
	unsigned int addr  =glb_mboot_ptr->mmap_addr;
	printf("mmap分布:\n",0);
	for(;(unsigned int)mmap<addr+length;mmap++)
	{
		printf("addr  : ",0);
		printf("%h",mmap->base_addr_high);
		printf("%h",mmap->base_addr_low);
		printf("            length: ",0);
		printf("%h",mmap->length_high);
		printf("%h",mmap->length_low);
		printf("\ntype: ",0);
		printf("%h",mmap->type);
		printf("\n",0);
	}
}
