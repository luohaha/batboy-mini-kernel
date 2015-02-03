/*
这个是物理内存分配，
virtual_memory.h
定义的是虚拟内存分配，
所以，kernel采用的是虚拟内存分配（分页）
,最后，虚拟线性地址都要通过分页机制转换为物理内存地址，也就是下面所定义的
*/
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "common.h"
#include "multiboot.h"
#define PAGE_SIZE 0x1000  //4KB
#define STACK_SIZE 8192	  //	
#define MAX_MEMORY_SIZE 0x20000000 //512MB
#define PAGE_NUMBER (MAX_MEMORY_SIZE/PAGE_SIZE)
//extern unsigned int page_count;//目前可以供使用的内存页面的数量
//extern unsigned int page_stack[STACK_SIZE];//可用页面起始地址栈

void print_memory();
extern unsigned char kern_start[];
extern unsigned char kern_end[];

void init_memory();
void memory_free(unsigned int addr);
unsigned int memory_alloc();

#endif
