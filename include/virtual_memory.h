#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include "common.h"
#include "memory_manager.h"
#include "descriptor_tables.h"
//内核在0xC0000000后地址运行
#define KERNEL_OFFSET 0xC0000000
/*
	以下是页表项结构，32bit，放在页表中，用来指向实际的地址
	31           12 | 11    9 | 8 7 | 6 | 5 | 4 3 | 2 | 1 | 0 
	frame address   | AVIAL   | RSVD| D | A | RSVD|U/S|R/W| P

	说明：
	P：如果当此页表在内存时，P = 1；
	R/W：如果=1，则可写，=0不可写
	U/S：如果=1，用户模式下的页面，=0，内核模式下的页面。
		（用户进程是不能访问内核模式下的页面的，需要系统调用）
	RSVD：保留
	A：CPU设置的，页面可行位
	D：传说中的脏位，用来指明内存中的数据是否被修改过（是否和磁盘中的数据
	   一致），对应不同的页表修改机制，脏位有不同的用法。
	frame address：指向物理内存地址
*/
#define PAGE_P 0x1
#define PAGE_RW 0x2
#define PAGE_US 0x4
//虚拟分页的大小任然是4KB
//#define VM_PAGE_SIZE 0x1000//已经在memory_manger.h中定义
//对齐4KB
#define PAGE_MASK 0xFFFFF000
/*
定义页目录项，页表项的数据类型
～～说是数据类型，起始不就是一个32位空间罢了
*/
typedef unsigned int page_dir_entry;
typedef unsigned int page_table_entry;
/*
因为只能用一个页（4kb）来存储页目录，页表，
所以我们可以求出一个页内的页目录项数，和页表项数
*/
#define PDE_NUM (PAGE_SIZE/sizeof(unsigned int))
#define PTE_NUM  (PAGE_SIZE/sizeof(unsigned int))
//映射512MB需要128个页表，
//因为一个页面（保存这页表）能存放4KB/4B=1K个页表项，对应出去是1K×4KB=4MB的内存
//所以512MB需要128个这种页面,实际上页目录能存1K个页表，但这里只用到128个～～
//注意：这里的只管理512MB指的是物理内存，但线性逻辑空间有
//4GB
#define PAGE_DIR_ENTRY_NUM 128
extern page_dir_entry pde_kern[PDE_NUM];//内核页目录数组
/*
一下宏函数用于获取目录项号，页表项号，和偏移地址
*/
#define GET_PDE_NUM(x) (((x) >> 22) & 0x3FF)
#define GET_PTE_NUM(x)  (((x) >> 12) & 0x3FF)
#define GET_OFFSET(x)  ((x) & 0xFFF)

//************函数定义×××××××××//

void init_virtual_memory();
void switch_page_dir();//切换页目录（页目录放在CR3）
//将虚拟地址映射到物理地址
void map(page_dir_entry *pde_now,unsigned int virtual_addr,unsigned int physics_addr,unsigned int flags);
//取消对虚拟地址的映射
void unmap(page_dir_entry *pde_now,unsigned int virtual_addr);
//如果虚拟地址已经映射，返回1
//如果physics_addr不为空指针，则把物理地址写入physics_addr
unsigned int get_mapping(page_dir_entry *pde_now,unsigned int virtual_addr,unsigned int *physics_addr);
void page_fault(registers_t *regs);
//切换进程未实现，暂时用不到
page_dir_entry *clone_dir(page_dir_entry*);//复制页目录项
#endif
