#include "common.h"
#include "virtual_memory.h"
#include "memory_manager.h"
#include "descriptor_tables.h"
#include "memory.h"
#include "monitor.h"
/*
	1.当分页启动后，每次使用内存的地址时，系统自动转换为物理地址，也就是说，系统会默认你输入的是线性逻辑地址
	2.页目录项和页表项中存的一定是物理地址，规定的～～
*/
//定义一个全局的页目录数组，大小4KB/4B=1K
page_dir_entry pde_kern[PDE_NUM] __attribute__ ((aligned(PAGE_SIZE)));
//定义一个二维的页表数组，第一维对应各个页表(128个)，第二维对应
//每个页表中的页表项。
static page_table_entry pte_kern[PAGE_DIR_ENTRY_NUM][PTE_NUM] __attribute__ ((aligned(PAGE_SIZE)));
void init_virtual_memory()
{
int i,j;
int PDE_BEGIN = GET_PDE_NUM(KERNEL_OFFSET);
for(i = PDE_BEGIN, j=0 ; i < PDE_BEGIN+PAGE_DIR_ENTRY_NUM;i++,j++)
{
	//这里需要将虚拟内存转换为物理内存
	pde_kern[i] = ((unsigned int) pte_kern[j]-KERNEL_OFFSET)|PAGE_P|PAGE_RW;
}
//对二维数组中的每个页表项赋值地址，128*1024个,
//128*1024*4KB=512MB
unsigned int *pte=(unsigned int *)pte_kern;
for(j=1;j<PAGE_DIR_ENTRY_NUM*PTE_NUM;j++)
{
	pte[j] = (j<<12)|PAGE_P|PAGE_RW;	
}
unsigned int new_pm_addr=(unsigned int)pde_kern-KERNEL_OFFSET;
register_int_handler(14,&page_fault);
switch_page_dir(new_pm_addr);
}
void switch_page_dir(unsigned int addr)
{
	asm volatile ("mov %0, %%cr3" : : "r" (addr));
}
void map(page_dir_entry *pde_now, unsigned int virtual_addr,unsigned physics_addr,unsigned int flags)
{
	unsigned int dir_num=GET_PDE_NUM(virtual_addr);
	page_table_entry *pte=(page_table_entry *)(pde_now[dir_num]&PAGE_MASK);
	if(!pte)
	//没有定义
	{
		//分配一个新的页表
		pte=(page_table_entry *) memory_alloc();
		//注册新页表到页目录项
		pde_now[dir_num]=(unsigned int)pte | PAGE_P | PAGE_RW;
		//切换到系统能够理解的线性逻辑地址
		pte=(page_table_entry *)((unsigned int)pte+KERNEL_OFFSET);
		memset(pte,0,PAGE_SIZE);
	}
	else
	{
		pte=(page_table_entry *)((unsigned int)pte+KERNEL_OFFSET);
	}
	//完成映射
	pte[GET_PTE_NUM(virtual_addr)]=(physics_addr & PAGE_MASK)|flags;
	//通知cpu
	asm volatile ("invlpg (%0)" : : "a" (virtual_addr));
	
}
void unmap(page_dir_entry *pde_now,unsigned int virtual_addr)
{
	unsigned int dir_num = GET_PDE_NUM(virtual_addr);
	page_table_entry *pte =(page_table_entry *)(pde_now[dir_num]&PAGE_MASK);
	if(!pte)
	//没有
		return;
	pte=(page_table_entry *)((unsigned int)pte+KERNEL_OFFSET);
	pte[GET_PTE_NUM(virtual_addr)] = 0;
	asm volatile ("invlpg (%0)" : : "a" (virtual_addr)); 
}
//判断虚拟地址是否以及映射了物理地址，没有就返回0，
//有，就返回1，并给physics_addr赋值
unsigned int get_mapping(page_dir_entry *pde_now,unsigned int virtual_addr,unsigned int *physics_addr)
{
	unsigned int dir_num = GET_PDE_NUM(virtual_addr);
	page_table_entry *pte =(page_table_entry *)(pde_now[dir_num]&PAGE_MASK);
	if(!pte)
	//没有
		return 0;
	pte=(page_table_entry *)((unsigned int)pte+KERNEL_OFFSET);
	if(pte[GET_PTE_NUM(virtual_addr)]!=0&&physics_addr)
	{
		*physics_addr=pte[GET_PTE_NUM(virtual_addr)]&PAGE_MASK;
		return 1;
	}
	return 0;
}


void page_fault(registers_t *regs)
{
	unsigned int cr2=0;
	asm volatile ("mov %%cr2, %0" : "=r" (cr2));
	printf("virtual err addr:   ",0);
	printf("%d",cr2);
	printf("\n",0);
	return;
}
