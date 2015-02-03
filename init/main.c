/*
	这就是内核最开始的地方，也是操作系统开始的地方
*/
#include "common.h"
#include "memory_manager.h"
#include "virtual_memory.h"
int main();
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
	unsigned int kern_stack_top=((unsigned int)kern_stack+STACK_SIZE) & 0xFFFFFFF0;
	asm volatile ("mov %0, %%esp\n\t"
			"xor %%ebp, %%ebp" : : "r" (kern_stack_top));
	glb_mboot_ptr=glb_mboot_ptr_tmp + KERNEL_OFFSET;
	//glb_mboot_ptr=glb_mboot_ptr_tmp;
	main();
}

int main()
{
	init_descriptor_tables();
	monitor_clear();
	printf("hello!\n",0);
	printf("%h",kern_start);
	printf("\n",0);
	printf("%h",kern_end);
	printf("\n",0);
	printf("memory use(KB):  ",0);
	printf("%h",(kern_end-kern_start+1023)/1024);
	asm volatile ("sti");
	print_memory();	
	//asm volatile ("int $0x3");
	//init_timer(50);
	init_memory();
	printf("\n---------------------------------------\n",0);
//	printf("now usefull pages is:  ",0);
//	printf("%d",page_count);
	printf("\n",0);
        int i;
	for(i=0;i<4;i++)
	{
		printf("0x",0);
		printf("%h",memory_alloc());
		printf("\n",0);
	}
	while(1)
	{
		asm volatile ("hlt");
	}
}
