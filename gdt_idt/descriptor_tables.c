/*
	初始化gdt与idt，定义默认的isr和irq
*/
#include "common.h"
#include "descriptor_tables.h"
#include "monitor.h"
//extern void gdt_flush(unsigned int);
//extern void idt_flush(unsigned int);
extern void init_idt();
extern void init_gdt();




void init_descriptor_tables()
{
	init_gdt();
	init_idt();
}





