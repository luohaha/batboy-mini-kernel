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

%macro IRQ 2
	[GLOBAL irq%1]
	irq%1:
		cli;
		push byte 0
		push byte %2
		jmp irq_common_stub
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
ISR_NOERR 128

IRQ   0,    32 	; 电脑系统计时器
IRQ   1,    33 	; 键盘
IRQ   2,    34 	; 与 IRQ9 相接，MPU-401 MD 使用
IRQ   3,    35 	; 串口设备
IRQ   4,    36 	; 串口设备
IRQ   5,    37 	; 建议声卡使用
IRQ   6,    38 	; 软驱传输控制使用
IRQ   7,    39 	; 打印机传输控制使用
IRQ   8,    40 	; 即时时钟
IRQ   9,    41 	; 与 IRQ2 相接，可设定给其他硬件
IRQ  10,    42 	; 建议网卡使用
IRQ  11,    43 	; 建议 AGP 显卡使用
IRQ  12,    44 	; 接 PS/2 鼠标，也可设定给其他硬件
IRQ  13,    45 	; 协处理器使用
IRQ  14,    46 	; IDE0 传输控制使用
IRQ  15,    47 	; IDE1 传输控制使用

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

[GLOBAL irq_common_stub]
[EXTERN irq_handler]

irq_common_stub:
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
   	call irq_handler
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
