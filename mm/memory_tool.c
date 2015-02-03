#include "memory.h"

void memset(void *dest, unsigned char val, unsigned int len)
{
	unsigned char *dst = (unsigned char *)dest;

	for ( ; len != 0; len--) {
		*dst++ = val;
	}
}
