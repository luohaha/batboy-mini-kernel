/*
	这就是内核最开始的地方，也是操作系统开始的地方
*/
#include "common.h"
#include "memory_manager.h"
#include "virtual_memory.h"
#include "memory_pool.h"
#include "fs.h"
#include "initrd.h"
#include "sched.h"

extern unsigned int placement_address;
//extern unsigned int kern_stack_top;
int main();
int haha=0;
//void stest();
multiboot_struct *glb_mboot_ptr;//切换到分页后要用的multiboot指针
char kern_stack[STACK_SIZE];
//建立临时页目录项，页表项，用来指向两个页
__attribute__((section(".init.data"))) page_dir_entry *pde_tmp = (page_dir_entry *)0x1000;
__attribute__((section(".init.data"))) page_dir_entry *pte_low = (page_dir_entry *)0x2000;
__attribute__((section(".init.data"))) page_dir_entry *pte_high = (page_dir_entry *)0x3000;

__attribute__((section(".init.text"))) void entry()
{
	//init_descriptor_tables();
	pde_tmp[0] = (unsigned int)pte_low|PAGE_P|PAGE_RW;
	pde_tmp[GET_PDE_NUM(KERNEL_OFFSET)]=(unsigned int)pte_high|PAGE_P|PAGE_RW;
	int i;
	//4KB/4B=1024
	for(i = 0; i<1024; i++)
	{
		pte_low[i] = (i<<12) | PAGE_P | PAGE_RW;
	}
	for(i = 0; i<1024; i++)
	{
		pte_high[i] = (i<<12) |PAGE_P | PAGE_RW;
	}
	//设置页目录的地址，向CR3赋值
	asm volatile ("mov %0, %%cr3" : : "r" (pde_tmp));
	//设置CR0的PG位为1，PG位是CR0的最高位
        unsigned int cr0=0;
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	cr0|=0x80000000;
	asm volatile ("mov %0, %%cr0" : : "r" (cr0));
	//切换内核栈
	kern_stack_top=((unsigned int)kern_stack+STACK_SIZE) & 0xFFFFFFF0;
	asm volatile ("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (kern_stack_top));
	glb_mboot_ptr=glb_mboot_ptr_tmp + KERNEL_OFFSET;
	//glb_mboot_ptr=glb_mboot_ptr_tmp;
	main();
}

void stest()
{
	unsigned int i=0;
	while(1)
	{
	i++;
	if(haha==0)
	{
		printf("a",0);
		haha=1;
	}
	if(i==0x1000000)
		break;
	}
	return ;
}
void stest2()
{
	unsigned int i=0;
	while(1)
	{
	i++;
	if(haha==1)
	{
		printf("c",0);
		haha=2;
	}
	if(i==0x10000)
		break;
	}
	return ;
}
int main()
{
	init_descriptor_tables();
	monitor_clear();
	printf("hello batboy!\n",0);
	printf("\n--------------memory-------------------\n",0);
	printf("%h",kern_start);
	printf("\n",0);
	printf("%h",kern_end);
	printf("\n",0);
	printf("memory use(KB):  ",0);
	printf("%h",(kern_end-kern_start+1023)/1024);
	glb_mboot_ptr->flags|=0x8;
	unsigned int initrd_location = *((unsigned int*)glb_mboot_ptr->mods_addr);
	unsigned int initrd_end = *(unsigned int*)(glb_mboot_ptr->mods_addr+4);
	//placement_address = initrd_end;
//	asm volatile ("sti");
//	print_memory();	
	//asm volatile ("int $0x3");
	
	init_memory();
	init_virtual_memory();
	init_pool();
	fs_root = initialise_initrd(initrd_location+KERNEL_OFFSET);
	printf("\n--------------memory pool---------------------\n",0);
//	printf("now usefull pages is:  ",0);
//	printf("%d",page_count);
//	printf("\n",0);
 /*       int i;
	for(i=0;i<4;i++)
	{
		printf("0x",0);
		printf("%h",memory_alloc());
		printf("\n",0);
	}*/
	pool_test();
	printf("\n--------------read file----------------------\n",0);
	int i = 0;
    	struct dirent *node = 0;
//	printf("%h",(unsigned int)glb_mboot_ptr->mods_count);
//	printf("loaded\n",0);
    	while ( (node = readdir_fs(fs_root, i)) != 0)
	//读出根目录下的所有文件，从0号开始
    	{
        	printf("Found file ",0);
        	printf(node->name,0);
        	fs_node_t *fsnode = finddir_fs(fs_root, node->name);

        	if ((fsnode->flags&0x7) == FS_DIRECTORY)
        	{
            		printf("\n\t(directory)\n",0);
        	}
        	else
        	{
            		printf("\n\t contents:  ",0);
            		char buf[256];
			char *change="can i fuck you all the time?i don't think so, but i have to try!\n yes you can!";
			unsigned int zs = write_fs(fsnode,0,256,change);
            		unsigned int sz = read_fs(fsnode, 0, 256, buf);
			printf("write size:  ",0);
			printf("%d",zs);
			printf("read size:  ",0);
			printf("%d",sz);
			printf("\n");
            		int j;
            		for (j = 0; j < sz+15; j++)
                		monitor_put(buf[j]);
            
            		printf("\n",0);
        	}
        	i++;
    	}
	printf("---------------------------------------\n",0);
	init_schedule();
	asm volatile ("sti");
	init_timer(200);
//	kernel_thread(stest+KERNEL_OFFSET,0);
	kernel_thread(stest);
	kernel_thread(stest2);
	while(1)
	{
//		asm volatile ("hlt");
		if(haha==2)
		{
			printf("b",0);
			haha=0;
		}
	}
}

