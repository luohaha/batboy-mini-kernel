/*系统调用部分未完成!!!
  bug: switch_usermode()函数执行时,会产生13号中断,就是常规保护错误

*/
#include "syscall.h"
#include "monitor.h"
#include "descriptor_tables.h"
#include "sched.h"
#include "memory_manager.h"
#include "virtual_memory.h"
#include "memory_pool.h"
extern task_t *current_task;//当前运行任务
extern unsigned int kern_stack_top;//初始主任务的堆栈顶

static void syscall_handler(registers_t *regs);
static void *syscall_function[1]={
	&printf,
};
unsigned int function_number=1;
void switch_usermode()
//切换到用户模式，用户模式的代码段为0x18，在将低两位（RPL）全部置一,0x18|0x3=0x1b
//同理数据段为0x20，然后RPL置一，0x20|0x3=0x23
//bug: 嵌入at&t汇编在执行时会产出13号中断
//按理来说,iret后eip,cs和eflags应该被弹出,然后赋值.接着在执行label 1 的下面程序(已经进入user模式),
//但是不知为什么,会出现13号中断错误(常规保护错误).
//date:  2015-02-09
{
	//set_kernel_stack(kern_stack_top);
	set_kernel_stack((unsigned int)current_task+STACK_SIZE);
	asm volatile("  \
     	cli; \
     	mov $0x23, %ax; \
     	mov %ax, %ds; \
     	mov %ax, %es; \
     	mov %ax, %fs; \
     	mov %ax, %gs; \
                   \
     	movl %esp, %eax; \
     	pushl $0x23; \
     	pushl %eax; \
     	pushf; \
	popl %eax; \
	orl $0x200, %eax; \
	pushl %eax; \
     	pushl $0x1B; \
     	pushl $1f; \
     	iret; \
   	1: \
     	");
//	asm volatile("  \
     	cli; \
     	mov $0x23, %ax; \
     	mov %ax, %ds; \
     	mov %ax, %es; \
     	mov %ax, %fs; \
     	mov %ax, %gs; \
                   \
     	mov %esp, %eax; \
     	mov $0x23, %ss; \
     	movl %eax, %esp; \
     	pushf; \
	popl %eax; \
	orl $0x200, %eax; \
	pushl %eax; \
	popf; \
     	mov $0x1B, %cs; \
     	movl $1f, %eip; \
     	ret; \
   	1: \
     	");
}


void init_syscall()
{
	//注册系统调用到中断128号(模仿linux)
	register_int_handler(128,&syscall_handler);
}

void syscall_handler(registers_t *regs)
{
	//if(regs->eax>0)
		return;
	void *now_function=syscall_function[regs->eax];
	//用汇编语言实现1.压入可能用到的参数.2.调用函数.3.返回返回值~~
	int retval;//返回值
	asm volatile ("\
			push %1;\
			push %2;\
			push %3;\
			push %4;\
			push %5;\
			call *%6;\
			pop %%ebx;\
			pop %%ebx;\
			pop %%ebx;\
			pop %%ebx;\
			pop %%ebx;\
			":"=a"(retval):"r" (regs->edi),"r" (regs->esi),"r" (regs->edx),"r" (regs->ecx),"r" (regs->ebx),"r" (now_function));
	regs->eax=retval;
}
void syscall_printf(char *s,unsigned int num)
{
	int rt;
	int he=0;
	asm volatile ("int $0x80" :"=a" (rt): "0" (he),"b" ((int)s),"c" ((int)num));
//	asm volatile("int $0x80;");
	return;
}
