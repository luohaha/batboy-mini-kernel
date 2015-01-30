%macro ISR_NOERR 1
	[GLOBAL isr%1]
	isr%1:
		cli
		push byte 0
		push byte %1
		jmp  isr_common_stub
%endmacro

%macro ISR_ERR  1
	[GLOBAL isr%1]
	isr%1:
		cli;
		push byte %1
		jmp  isr_common_stub
%endmacro

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR 17
ISR_NOERR 18
ISR_NOERR 19

ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
; 32 ~ 255 用户自定
;ISR_NOERR 255

[GLOBAL isr_common_stub]
[EXTERN isr_handler]

isr_common_stub:
	pusha
	mov ax,ds
	push eax
	mov ax, 0x10  ; load the kernel data segment descriptor
   	mov ds, ax
   	mov es, ax
   	mov fs, ax
   	mov gs, ax
        mov ss, ax

	push esp
   	call isr_handler
	add esp, 4

   	pop ebx        ; reload the original data segment descriptor
   	mov ds, bx
   	mov es, bx
   	mov fs, bx
   	mov gs, bx
	mov ss, bx

   	popa                     ; Pops edi,esi,ebp...
   	add esp, 8     ; Cleans up the pushed error code and pushed ISR number
	sti
   	iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
