#include "common.h"
#include "descriptor_tables.h"
#include "monitor.h"
#include "syscall.h"
static void gdt_set_gate(unsigned int,unsigned int,unsigned int,unsigned char,unsigned char);
static void write_tss(unsigned int num, unsigned short ss0,unsigned int esp0);
extern void tss_flush();
void init_gdt();
gdt_entry gdt_entries[6];//定义5个段
gdt_ptr   the_gdt_ptr;   //定义段表开始地址及长度
tss_t     tss_entry;//定义一个tss段
void init_gdt()
{
	the_gdt_ptr.limit=(sizeof(gdt_entry)*6)-1;
	the_gdt_ptr.base=(unsigned int)&gdt_entries;
	
	gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
        gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
        gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
        gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
        gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
	write_tss(5,0x10,0x0);//add tss段
	gdt_flush((unsigned int)&the_gdt_ptr);
	tss_flush();
}
static void gdt_set_gate(unsigned int num,unsigned int base,unsigned int limit,unsigned char access,unsigned char gran)
{
	//用了定义entry具体内容的函数
	gdt_entries[num].base_low    = (base & 0xFFFF);
   	gdt_entries[num].base_middle = (base >> 16) & 0xFF;
   	gdt_entries[num].base_high   = (base >> 24) & 0xFF;

   	gdt_entries[num].limit_low   = (limit & 0xFFFF);
   	gdt_entries[num].granularity = (limit >> 16) & 0x0F;

   	gdt_entries[num].granularity |= gran & 0xF0;
   	gdt_entries[num].access      = access;
}

static void write_tss(unsigned int num, unsigned short ss0, unsigned int esp0)
{
	unsigned int base = (unsigned int) &tss_entry;
	unsigned int limit=base+sizeof(tss_t);
	gdt_set_gate(num,base,limit,0xE9,0x00);
	memset(&tss_entry,0,sizeof(tss_t));
	tss_entry.ss0=ss0;
	tss_entry.esp0=esp0;
	//因为首先是内核模式，所以代码段是0x08数据段是0x10
	//将后两位(rpl)置1，所以是0x0b和0x13
	tss_entry.ss=tss_entry.ds=tss_entry.es=tss_entry.gs=tss_entry.fs=0x13;
	//代码段
	tss_entry.cs=0x0b;
}
void set_kernel_stack(unsigned int stack)
{
	tss_entry.esp0=stack;
}
