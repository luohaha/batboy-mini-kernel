#include "memory.h"

void memset(void* addr,unsigned char ch,unsigned int size)
{
	unsigned int *add=(unsigned int*)((unsigned int)addr & (unsigned int)0x000000ff);
	//unsigned int *add=(unsigned int*)addr;
	unsigned int i=0;
	for(i=0;i<size;i++)
	{
		*(add+i)=ch;
	}
	return;
}
