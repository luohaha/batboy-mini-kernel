#ifndef SYSCALL_H
#define SYSCALL_H
#include "common.h"
#include "monitor.h"

/////定义一个TSS段
typedef struct tss_struct{
   unsigned int prev_tss;   // 上一个tss，硬件任务切换时会用到tss链
   unsigned int esp0;       // 内核模式下的堆栈指针
   unsigned int ss0;        // 内核模式下的栈段地址
   unsigned int esp1;       //
   unsigned int ss1;        //ring1和2都没有用
   unsigned int esp2;       //
   unsigned int ss2;
   unsigned int cr3;
   unsigned int eip;
   unsigned int eflags;
   unsigned int eax;
   unsigned int ecx;
   unsigned int edx;
   unsigned int ebx;
   unsigned int esp;
   unsigned int ebp;
   unsigned int esi;
   unsigned int edi;
   unsigned int es;         // The value to load into ES when we change to kernel mode.
   unsigned int cs;         // The value to load into CS when we change to kernel mode.
   unsigned int ss;         // The value to load into SS when we change to kernel mode.
   unsigned int ds;         // The value to load into DS when we change to kernel mode.
   unsigned int fs;         // The value to load into FS when we change to kernel mode.
   unsigned int gs;         // The value to load into GS when we change to kernel mode.
   unsigned int ldt;        // Unused...
   unsigned short trap;
   unsigned short iomap_base;	
} __attribute__((packed));

typedef struct tss_struct tss_t;

void switch_usermode();//切换到用户模式
//extern void tss_flush();//载入tss段，在gdt.s中实现
void init_syscall();//初始化系统调用
void syscall_printf(char*,unsigned int);
void set_kernel_stack(unsigned int stack);
#endif
