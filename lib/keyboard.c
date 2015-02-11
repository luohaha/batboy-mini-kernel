#include "common.h"
#include "memory_manager.h"
#include "virtual_memory.h"
#include "memory_pool.h"
#include "fs.h"
#include "initrd.h"
#include "sched.h"
#include "syscall.h"
#include "keyboard.h"
#include "monitor.h"
int haha=0;
extern fs_node_t *fs_root; // The root of the filesystem.
static void keyboard_handler();//键盘处理函数
static void exe_cmd(char *todo);//执行命令行函数
static char keytable[0x54] = {0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08, '\t','Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',   0,   'A', 'S','D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', 0,0,   '\\', 'Z', 'X', 'C', 'V','B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0, 0,0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1','2', '3', '0', '.'};
static int caps;//0时为小写字母
//用于存储命令行
static char cmd[64];
//记录cmd的长度
static int cmd_len;
//////////******命令程序*********//////////
static void show_memory();
static void help();
static void show_pool();
static void show_file();
static void show_task();
/////////////////多任务的展示函数
void stest()
{
	unsigned int i=0;
	while(1)
	{
	i++;
	if(haha==0)
	{
		printf("a",0);
		haha=1;
	}
	if(i==0x1000000)
		break;
	}
	return ;
}
void stest2()
{
	unsigned int i=0;
	while(1)
	{
	i++;
	if(haha==1)
	{
		printf("c",0);
		haha=0;
	}
	if(i==0x1000000)
		break;
	}
	return ;
}
/////////////////////////////////////
void init_keyboard()
//键盘初始化,将键盘处理函数注册到
//IRQ1,33号中断
{
	printf("batboy$$$>>>>",0);
	register_int_handler(33,keyboard_handler);
	caps=0;
	cmd_len=0;
}
void keyboard_handler()
{
	unsigned char ret;
	ret = inb(0x60);//读入键盘输入
	if(ret&0x80)
	//松开按键时的编码
	{
		if(ret==0xba)
		//caps lock键
		{
			caps^=1;
		}
				
	}
	else
	//按下按键时的编码
	{
		if(caps==0&&keytable[ret]>=65&&keytable[ret]<=90)
		{
			monitor_put(keytable[ret]+32);
		}
		else
			monitor_put(keytable[ret]);
		//执行命令行
		if(keytable[ret]!='\n')
			cmd[cmd_len++]=keytable[ret];
		else
		{
			exe_cmd(cmd);
			printf("\n",0);
			cmd_len=0;
			cmd[cmd_len]=' ';
			printf("batboy$$$>>>>",0);
		}
	}
}
void exe_cmd(char *todo)
{
	switch(todo[0])
	{
		case 'M': 
			show_memory();
			break;
		case 'H':
			help();
			break;
		case 'P':
			show_pool();
			break;
		case 'F':
			show_file();
			break;
		case 'T':
			show_task();
		//	printf('\n',0);
			break;
		default:
			printf("wrong command!\n",0);
			printf("please input 'help' for help\n",0);
			break;
	}
}
void show_memory()
{
	printf("\n--------------memory-------------------\n",0);
	printf("code start: ",0);
	printf("%h",kern_start);
	printf("\n",0);
	printf("code end:   ",0);
	printf("%h",kern_end);
	printf("\n",0);
	printf("code's memory use(KB):  ",0);
	printf("%h",(kern_end-kern_start+1023)/1024);
	printf("\n",0);
}
void help()
{
	printf("------------command list---------------\n",0);
	printf("memory:   show memory use\n",0);
	printf("pool  :   show kmalloc and kfree\n",0);
	printf("task  :   show multi-task, print 'a' and 'c' at the same time\n",0);
	printf("file  :   show file write and read\n",0);
	printf("\n",0);
}
void show_pool()
{
	printf("\n--------------memory pool---------------------\n",0);
//	printf("now usefull pages is:  ",0);
//	printf("%d",page_count);
//	printf("\n",0);
 /*       int i;
	for(i=0;i<4;i++)
	{
		printf("0x",0);
		printf("%h",memory_alloc());
		printf("\n",0);
	}*/
	pool_test();
	printf("\n",0);
}
void show_file()
{
	printf("\n--------------read file----------------------\n",0);
	int i = 0;
    	struct dirent *node = 0;
//	printf("%h",(unsigned int)glb_mboot_ptr->mods_count);
//	printf("loaded\n",0);
    	while ( (node = readdir_fs(fs_root, i)) != 0)
	//读出根目录下的所有文件，从0号开始
    	{
        	printf("file:  ",0);
        	printf(node->name,0);
        	fs_node_t *fsnode = finddir_fs(fs_root, node->name);

        	if ((fsnode->flags&0x7) == FS_DIRECTORY)
        	{
            		printf("\n\t(directory)\n",0);
        	}
        	else
        	{
            		printf("\n\t contents:  ",0);
            		char buf[256];
			char *change="I will not give up hope, and i have to try!\n yes you can!";
			unsigned int zs = write_fs(fsnode,0,256,change);
            		unsigned int sz = read_fs(fsnode, 0, 256, buf);
			printf("write size:  ",0);
			printf("%d",zs);
			printf("read size:  ",0);
			printf("%d",sz);
			printf("\n",0);
            		int j;
            		for (j = 0; j < sz+15; j++)
                		monitor_put(buf[j]);
            
            		printf("\n",0);
        	}
        	i++;
    	}

}
void show_task()
{
	printf("---------------kernel thread test-------------------\n",0);
	
	init_timer(200);
	asm volatile ("sti");
	kernel_thread(stest);
	kernel_thread(stest2);

}
