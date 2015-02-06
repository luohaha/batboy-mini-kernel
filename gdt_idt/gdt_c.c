#include "common.h"
#include "descriptor_tables.h"
#include "monitor.h"
static void gdt_set_gate(unsigned int,unsigned int,unsigned int,unsigned char,unsigned char);
void init_gdt();
gdt_entry gdt_entries[5];//定义5个段
gdt_ptr   the_gdt_ptr;   //定义段表开始地址及长度
void init_gdt()
{
	the_gdt_ptr.limit=(sizeof(gdt_entry)*5)-1;
	the_gdt_ptr.base=(unsigned int)&gdt_entries;
	
	gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
        gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
        gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
        gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
        gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
	gdt_flush((unsigned int)&the_gdt_ptr);
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
