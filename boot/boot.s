;内核的开始地址

;魔数magic
MultiBoot_Magic EQU 0x1BADB002 ;这个是grub可以识别的规范
;flags可以指出引导程序支持的特性
MultiBoot_Flags EQU 0x00000003 ;第0位和1位设置为1是为了4kb对齐和men*_域包含可用内存信息
;magic，flags和checksum的和必须为0
MultiBoot_Checksum EQU -(MultiBoot_Magic+MultiBoot_Flags)

[BITS 32]      ;代码以32位编译
section .init.text  ;代码段开始的地方

	dd MultiBoot_Magic
	dd MultiBoot_Flags
	dd MultiBoot_Checksum

[GLOBAL start]        ;外部可见的内核代码开始入口
[GLOBAL glb_mboot_ptr_tmp];struct multiboot *变量
[EXTERN entry]   ;声明内核c代码的入口函数

start:
	cli		;关中断
	mov [glb_mboot_ptr_tmp], ebx
	mov esp,STACK_TOP;吧esp的值保存的栈顶
	and esp,0FFFFFFF0H;按16字节对齐
	mov ebp,0
	call entry

section .init.data   ;data段开始了，data就是已经被初始化的全局变量
stack:  times 1024 db 0 ;临时内核栈
STACK_TOP EQU $-stack-1 ;$代表当前地址
glb_mboot_ptr_tmp: dd 0

