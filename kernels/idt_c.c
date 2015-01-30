#include "common.h"
#include "descriptor_tables.h"
#include "monitor.h"
extern void memset(void* addr,unsigned char ch,unsigned int size);
void init_idt();
static void idt_set_gate(unsigned char,unsigned int,unsigned short,unsigned char);
idt_entry idt_entries[256];//定义256个中断
idt_ptr   the_idt_ptr;   //定义中断表的开始及长度
interrupt_handler_t interrupt_handlers[256];//中断处理函数的指针数组
void init_idt()
{
	memset((unsigned int *)&interrupt_handlers,0,sizeof(interrupt_handler_t)*256);
	the_idt_ptr.limit=sizeof(idt_entry)*256-1;
	the_idt_ptr.base=(unsigned int)&idt_entries;
	memset((unsigned int *)&idt_entries,0,sizeof(idt_entry)*256);
	idt_set_gate(0,(unsigned int)isr0,0x08,0x8E);
	idt_set_gate(1,(unsigned int)isr1,0x08,0x8E);
	idt_set_gate(2,(unsigned int)isr2,0x08,0x8E);
	idt_set_gate(3,(unsigned int)isr3,0x08,0x8E);
	idt_set_gate(4,(unsigned int)isr4,0x08,0x8E);
	idt_set_gate(5,(unsigned int)isr5,0x08,0x8E);
	idt_set_gate(6,(unsigned int)isr6,0x08,0x8E);
	idt_set_gate(7,(unsigned int)isr7,0x08,0x8E);
	idt_set_gate(8,(unsigned int)isr8,0x08,0x8E);
	idt_set_gate(9,(unsigned int)isr9,0x08,0x8E);
	idt_set_gate(10,(unsigned int)isr10,0x08,0x8E);
	idt_set_gate(11,(unsigned int)isr11,0x08,0x8E);
	idt_set_gate(12,(unsigned int)isr12,0x08,0x8E);
	idt_set_gate(13,(unsigned int)isr13,0x08,0x8E);
	idt_set_gate(14,(unsigned int)isr14,0x08,0x8E);
	idt_set_gate(15,(unsigned int)isr15,0x08,0x8E);
	idt_set_gate(16,(unsigned int)isr16,0x08,0x8E);
	idt_set_gate(17,(unsigned int)isr17,0x08,0x8E);
	idt_set_gate(18,(unsigned int)isr18,0x08,0x8E);
	idt_set_gate(19,(unsigned int)isr19,0x08,0x8E);
	idt_set_gate(20,(unsigned int)isr20,0x08,0x8E);
	idt_set_gate(21,(unsigned int)isr21,0x08,0x8E);
	idt_set_gate(22,(unsigned int)isr22,0x08,0x8E);
	idt_set_gate(23,(unsigned int)isr23,0x08,0x8E);
	idt_set_gate(24,(unsigned int)isr24,0x08,0x8E);
	idt_set_gate(25,(unsigned int)isr25,0x08,0x8E);
	idt_set_gate(26,(unsigned int)isr26,0x08,0x8E);
	idt_set_gate(27,(unsigned int)isr27,0x08,0x8E);
	idt_set_gate(28,(unsigned int)isr28,0x08,0x8E);
	idt_set_gate(29,(unsigned int)isr29,0x08,0x8E);
	idt_set_gate(30,(unsigned int)isr30,0x08,0x8E);
	idt_set_gate(31,(unsigned int)isr31,0x08,0x8E);

	idt_flush((unsigned int)&the_idt_ptr);
}
static void idt_set_gate(unsigned char num,unsigned int base,unsigned short sel,unsigned char flags)
{
	idt_entries[num].base_low=base&0xFFFF;
	idt_entries[num].base_high=(base>>16)&0xFFFF;

	idt_entries[num].sel=sel;
	idt_entries[num].always0=0;
	idt_entries[num].flags=flags;
}
void isr_handler(registers_t *regs)
{
	if(interrupt_handlers[regs->int_no])
	{
		interrupt_handlers[regs->int_no](regs);
	}
        else
        {
		printf("not int is:",0);
		printf("%d",regs->int_no);
	}
}
void register_int_handler(unsigned char num, interrupt_handler_t h)
{
	interrupt_handlers[num]=h;
}
