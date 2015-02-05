#include "memory.h"

void memset(void *dest, unsigned char val, unsigned int len)
{
	unsigned char *dst = (unsigned char *)dest;

	for ( ; len != 0; len--) {
		*dst++ = val;
	}
}
void memcpy(unsigned char *str1,unsigned char *str2,unsigned int len)
//把str2赋值给str1
{
	while(len>0)
	{
		*str1++=*str2++;
		len--;
	}
}
int strcmp(unsigned char *str1,unsigned char *str2)
{
	while(*str1&&*str2&&(*str1++==*str2++));
	if(*str1=='\0'&&*str2=='\0')
		return 0;//成功
	else if(*str1=='\0')
		return 1;
	else
		return -1;
}
unsigned char* strcpy(unsigned char *str1,unsigned char *str2)
//将str2复制给str1，并返回str1原来的值
{
	unsigned char *tmp=str1;
	while(*str2)
	{
		*str1++=*str2++;
	}
	*str1='\0';
	return tmp;
}
int strlen(unsigned char *c)
{
	const unsigned char *eos = c;
        while (*eos++)
	      ;
	return (eos - c - 1);
}
