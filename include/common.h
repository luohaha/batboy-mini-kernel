#ifndef COMMON_H
//如果没有定义过这个的话～～ifndef用于判断是否重复定义
#define COMMON_H

#ifndef NULL
	#define NULL 0
#endif

#ifndef TRUE
	#define TRUE 1
	#define FALSE 0
#endif

void outb(unsigned short port,unsigned char value);
unsigned char inb(unsigned short port);
unsigned short inw(unsigned short port);

#endif
