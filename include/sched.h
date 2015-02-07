#ifndef SCHED_H
#define SCHED_H

#include "common.h"
#include "memory_manager.h"
#include "virtual_memory.h"
#include "memory_pool.h"

#define TASK_RUNNING      0       //任务可以被调度成为当前任务，就绪状态
#define TASK_SLEEP        1       //睡眠，或叫做阻塞
#define TASK_ZIMBIE       2       //任务去世，但id未注销
#define TASK_STOPPED      4       //用于调试
/*
每个任务都有一个PCB结构和一片用于系统空间堆栈的存储空间
一共两个STACK_SIZE=2*页表=2*4KB
PCB放在低端，堆栈则是从高往下
*/
typedef struct context{//任务的上下文
	unsigned int esp;
	unsigned int ebp;
	unsigned int ebx;
	unsigned int esi;
	unsigned int edi;
	unsigned int eflags;
} context_t;

typedef struct task_struct{
	//TCB，任务控制块
	unsigned int pid;//任务号
	unsigned char state;//状态
	context_t  context;//上下文
	page_dir_entry *mm;//页目录项起始地址，轻量级进程用不到
	struct task_struct *next;//下一个任务
} task_t;

extern unsigned int kern_stack_top;//初始主任务的堆栈顶
extern unsigned int current_pid; //当前运行的任务号
extern task_t *running_task_queue;//就绪任务队列
extern task_t *sleep_task_queue;//阻塞任务队列
extern task_t *current_task;//当前运行任务
int kernel_thread(int (*fn)(void*));
void kthread_exit();

void init_schedule();//初始化调度
void schedule();//调度
void task_switch(context_t *prev, context_t *next);//任务切换
//
int fork();
int getpid();
#endif
