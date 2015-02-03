#include "monitor.h"
#include "virtual_memory.h"
unsigned short cursor_x=0;
unsigned short cursor_y=0;
unsigned short *video_memory=(unsigned short*)(0xB8000+KERNEL_OFFSET);
//unsigned short *video_memory=(unsigned short*)(0xB8000);
static void move_cursor()
//刷新光标位置
{
	/*
	port(0x3D4)+command(14)=>发高地址=>发给0x3D5
		    command(15)=>发低地址
	*/
	unsigned short location=cursor_y*80+cursor_x;
	outb(0x3D4,14);
	outb(0x3D5,location>>8);
	outb(0x3D4,15);
	outb(0x3D5,location);
}
static void scroll()
{
	unsigned char color=(0xf<<4)|(0x4&0x0f);
	unsigned short blank=0x20|(color<<8);//空格
	if(cursor_y>=25)
	//当到达底部时，向上滚动
	{
		int i;
		for(i=0;i<24*80;i++)
		{
			video_memory[i]=video_memory[i+80];
		}
		for(i=24*80;i<25*80;i++)
		{
			video_memory[i]=blank;
		}
	cursor_y=24;
	}
}
void monitor_put(char c)
{
	unsigned char backcolor=0xf;//背景色为白色
	unsigned char forecolor=0x4;//背景色为红色
	unsigned short color=(backcolor<<4)|(forecolor&0x0f);
	unsigned short *location;//要存放光标所在处的地址
        if(c==0x08&&cursor_x>=0)
	{
		//退格键
		cursor_x--;
	}
	else if(c==0x09)
	{
		//tab键
		cursor_x=(cursor_x+8)&~7;
	}
	else if(c=='\r')
	{
		cursor_x=0;
	}
	else if(c=='\n')
	{
		cursor_x=0;
		cursor_y++;
	}
	else if(c>=' ')
	{
		location=video_memory+(cursor_y*80+cursor_x);//光标处地址
		*location=c|(color<<8);//该显示的值
		cursor_x++;
	}

	if(cursor_x>=80)
	{
		cursor_x=0;
		cursor_y++;
	}
	scroll();
	move_cursor();//刷新显示
}
void monitor_clear()
{
	unsigned char color=(0xf<<4)|(0x4&0x0f);
	unsigned short blank=0x20|(color<<8);//空格
	int i;
	for(i=0;i<25*80;i++)
	{
		video_memory[i]=blank;
	}
	cursor_x=0;
	cursor_y=0;
	move_cursor();//刷新显示
}
void monitor_str(char *c)
{
	int i=0;
	while(c[i])
	{
		monitor_put(c[i++]);
	}
}
void monitor_hex(unsigned int c)
{
	char str[8];
	int i=0;
	for(i=0;i<8;i++)
	{
		str[i]=(char)(c%16+48);
		if(str[i]>=':')
			str[i]+=7;
		c/=16;
	}	
	int j;
	for(j=7;j>=0;j--)
	{
		monitor_put(str[j]);
	}
}
void monitor_dec(unsigned int c)
{
	if(c==0)
	{
		monitor_put('0');
	}
	int i;
	unsigned int num=1000000000;
	int flag=0;
	for(i=0;i<10;i++)
	{
		int tmp=c/num;
		if(tmp!=0)
		{
			flag=1;
			char ch=(char)(tmp+48);
			monitor_put(ch);
		}
		else
		{
			if(flag==1)
				monitor_put('0');
		}
		c=c-num*tmp;
		num/=10;
	}
}
void printf(char *s,unsigned int c)
{
	if(s[0]!='%')
		monitor_str(s);
	else
	{
		if(s[1]=='d')
			monitor_dec(c);
		else if(s[1]=='h')
			monitor_hex(c);
	}
}
