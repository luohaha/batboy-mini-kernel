#include "common.h"
#include "sched.h"

int kernel_thread(int (*fn)(void*))
{
	task_t *new_task = (task_t *)kmalloc(STACK_SIZE);
	memset(new_task,0,sizeof(task_t));
	new_task->pid=current_pid++;
	new_task->state=TASK_RUNNING;
	new_task->mm=0;
	unsigned int *new_top = (unsigned int*)((unsigned int)new_task+STACK_SIZE);
	*(--new_top)=(unsigned int)kthread_exit;
	*(--new_top)=(unsigned int)fn;
	new_task->context.eflags=0x200;
	new_task->context.esp=(unsigned int)new_task+STACK_SIZE-2*sizeof(unsigned int);
	new_task->next=running_task_queue;
	task_t *point=running_task_queue;
	while(point->next!=running_task_queue)
		point=point->next;
	point->next=new_task;
	return new_task->pid;//返回新任务的id
}
///////////////
//////////////fork()未完成，任务切换有问题，切换后系统会崩溃
int fork()
{
	asm volatile("cli");
	task_t *new_task = (task_t *)kmalloc(STACK_SIZE);
	memset(new_task,0,sizeof(task_t));
	//新建页目录
//	page_dir_entry *newdir = clone_dir(current_task->mm);
	new_task->pid=current_pid++;
	new_task->state=TASK_RUNNING;
	//页目录赋值
//	new_task->mm=newdir;
	new_task->mm=current_task->mm;
	new_task->context=current_task->context;
	new_task->next=running_task_queue;
	task_t *point=running_task_queue;
	while(point->next!=running_task_queue)
		point=point->next;
	point->next=new_task;
//	asm volatile ("sti");
	return new_task->pid;//返回新任务的id

}
void kthread_exit()
{
	//把当前任务从running_task_queue队列中移除
	task_t *point=running_task_queue;
	while(point->next!=current_task)
		point=point->next;
	point->next=current_task->next;
	//把完成的任务放入sleep_task_queue队列中，准备回收内存空间
	if(sleep_task_queue==0)
		sleep_task_queue=current_task;
	else
	{
		kfree(sleep_task_queue);
		sleep_task_queue=current_task;
	}
	//这里不能提前函数返回，因为堆栈中已经没有数，提前退出会出现访问页
	//错误
	while(1);
}
