/*
	主程序
*/
#include "common.h"
#include "memory_manager.h"
#include "virtual_memory.h"
#include "memory_pool.h"
#include "fs.h"
#include "initrd.h"
#include "sched.h"
#include "syscall.h"
#include "keyboard.h"
extern unsigned int placement_address;
//extern unsigned int kern_stack_top;
int main();
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

int main()
{
	init_descriptor_tables();
	monitor_clear();
	printf("welcome to batboy-mini-os!\n",0);
	printf("You can use 'help' to find more command!\n",0);
	
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
	asm volatile ("sti");
	init_schedule();
//	printf("--------------systerm call test-------------------\n",0);
	init_syscall();
//	switch_usermode();
//	asm volatile ("sti");
//	syscall_printf("I will not give up my hope!\n",0);	

	init_keyboard();
	while(1)
	{
		asm volatile ("hlt");
	/*	if(haha==2)
		{
			printf("b",0);
			haha=0;
		}*/
		
	}
}

