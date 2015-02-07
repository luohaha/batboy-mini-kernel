[GLOBAL task_switch]

task_switch: 
	mov eax, [esp+4] ;源任务上下文地址
	mov [eax+0], esp ;
	mov [eax+4], ebp
	mov [eax+8], ebx
	mov [eax+12], esi
	mov [eax+16], edi
	pushf            ;push eflags
	pop ecx
	mov [eax+20], ecx

	mov eax, [esp+8] ;新任务
	mov esp, [eax+0]
	mov ebp, [eax+4]
	mov ebx, [eax+8]
	mov esi, [eax+12]
	mov edi, [eax+16]
	mov eax, [eax+20]
	push eax
	popf            ;将堆栈顶的数值（eflags）弹出赋值

	ret
