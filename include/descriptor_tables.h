/*
*  段gdt
*
*/
#ifndef DESCRIPTOR_TABLES_H
#define DESCRIPTOR_TABLES_H

#include "common.h"

struct gdt_entry_struct
{
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_middle;
	unsigned char access;
	unsigned char granularity;
	unsigned char base_high;
}__attribute__((packed)) gdt_entry_struct;
typedef struct gdt_entry_struct gdt_entry;

struct gdt_ptr_struct
{
	unsigned short limit;
	unsigned int base;
}__attribute__((packed)) gdt_ptr_struct;
typedef struct gdt_ptr_struct gdt_ptr;

extern void gdt_flush(unsigned int);

/*
*   中断的实现的相关定义
*
*/
void init_descriptor_tables();

struct idt_entry_struct
{
	unsigned short base_low;//lower 16bit
	unsigned short sel;   //kernel seg selector
	unsigned char always0;
	unsigned char flags;
	unsigned short base_high;//high 16bit
}__attribute__((packed)) idt_entry_struct;
typedef struct idt_entry_struct idt_entry;

struct idt_ptr_struct
{
	unsigned short limit;
	unsigned int   base;
}__attribute__((packed)) idt_ptr_struct;
typedef struct idt_ptr_struct idt_ptr;
//
typedef struct registers
{
   unsigned int ds;                  // Data segment selector
   unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   unsigned int int_no, err_code;    // Interrupt number and error code (if applicable)
   unsigned int  eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
}registers_t; 

typedef void (*interrupt_handler_t)(registers_t *);

void isr0(); 		// 0 #DE 除 0 异常 
void isr1(); 		// 1 #DB 调试异常 
void isr2(); 		// 2 NMI 
void isr3(); 		// 3 BP 断点异常 
void isr4(); 		// 4 #OF 溢出 
void isr5(); 		// 5 #BR 对数组的引用超出边界 
void isr6(); 		// 6 #UD 无效或未定义的操作码 
void isr7(); 		// 7 #NM 设备不可用(无数学协处理器) 
void isr8(); 		// 8 #DF 双重故障(有错误代码) 
void isr9(); 		// 9 协处理器跨段操作 
void isr10(); 		// 10 #TS 无效TSS(有错误代码) 
void isr11(); 		// 11 #NP 段不存在(有错误代码) 
void isr12(); 		// 12 #SS 栈错误(有错误代码) 
void isr13(); 		// 13 #GP 常规保护(有错误代码) 
void isr14(); 		// 14 #PF 页故障(有错误代码) 
void isr15(); 		// 15 CPU 保留 
void isr16(); 		// 16 #MF 浮点处理单元错误 
void isr17(); 		// 17 #AC 对齐检查 
void isr18(); 		// 18 #MC 机器检查 
void isr19(); 		// 19 #XM SIMD(单指令多数据)浮点异常

// 20-31 Intel 保留
void isr20();
void isr21();
void isr22();
void isr23();
void isr24();
void isr25();
void isr26();
void isr27();
void isr28();
void isr29();
void isr30();
void isr31();
// 32 ~ 255 用户自定义异常
//void isr255();
extern void idt_flush(unsigned int);
void isr_handler(registers_t *regs);
void register_int_handler(unsigned char num, interrupt_handler_t h);
#endif
