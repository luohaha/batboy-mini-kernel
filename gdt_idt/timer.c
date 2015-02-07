#include "common.h"
#include "timer.h"
#include "descriptor_tables.h"
#include "monitor.h"
#include "sched.h"
unsigned int count=0;
void timer_count(registers_t *regs)
{
	count++;
	printf("\n",0);
//	printf("%d",regs->int_no);
//	printf("\n这是第几次?\n",0);
	printf("%d",count);
	printf("\n",0);
}
void sche(registers_t *regs)
{
	schedule();
}
void init_timer(unsigned int frequency)
{
	//printf("     准备计时\n",0);
	//把timer_count函数注册到中断32号口(irq0口)
	register_int_handler(32,sche);
	unsigned int div=1193180/frequency;
	//pit有四个端口,0x40~0x42是对应0~2频道,0x43对应命令口
	//准备传输
	outb(0x43,0x36);
	unsigned char low=(unsigned char)(div & 0xff);//低八位
	unsigned char high=(unsigned char)((div>>8) & 0xff);//高八位

	//分别传输
	outb(0x40,low);
	outb(0x40,high);
}

