#include "common.h"
#include "sched.h"
#include "memory_manager.h"
#include "virtual_memory.h"
#include "memory_pool.h"
unsigned int kern_stack_top;//初始主任务的堆栈顶
unsigned int current_pid=0; //当前运行的任务号
task_t *running_task_queue=0;//就绪任务队列
task_t *sleep_task_queue=0;//阻塞任务队列
task_t *current_task=0;//当前运行任务
void init_schedule()
{
	current_task = (task_t*)(kern_stack_top-STACK_SIZE);
	current_task->pid=current_pid++;
	current_task->state=TASK_RUNNING;
	//current_task->mm=(unsigned int)pde_kern-KERNEL_OFFSET;
	//未实现进程，所以还用不到独立内存空间
	current_task->mm=0;
	current_task->next=current_task;//只有一个任务
	running_task_queue=current_task;
}

void schedule()
{
	if(current_task&&current_task->next!=current_task)
//	if(current_task)
	{
		task_t *prev_task = current_task;
		current_task=current_task->next;
		task_switch(&(prev_task->context),&(current_task->context));
	//	switch_page_dir(current_task->mm);
	//	未实现进程，不用切换内存空间
	}
}

int getpid()
{
	return current_task->pid;
}
