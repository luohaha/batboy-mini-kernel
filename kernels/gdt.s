[GLOBAL gdt_flush]

gdt_flush:
	mov eax,[esp+4];
	lgdt [eax]
	;kernel data seg is 0x10,code seg is 0x08
	mov ax,0x10 ;
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	jmp 0x08:.flush
.flush:
	ret
