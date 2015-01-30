/*
*	链接脚本
*/
ENTRY(start)
SECTIONS
{
	/*
	*段起始，text是代码段，data是数据段
	*/
	. =0x100000;
	.text :
	{
		*(.text)
		. = ALIGN(4096);
	}
	.data :
	{
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}
	.bss :
	{
		*(.bss)
		. = ALIGN(4096);
	}
	.stab :
	{
		*(.stab)
		. = ALIGN(4096);
	}
	.stabstr :
	{
		*(.stabstr)
		. = ALIGN(4096);
	}
	/DISCARD/ : { *(.comment) *(.eh_frame) }
}
