#ifndef INITRD_H
#define INITRD_H

#include "common.h"
#include "fs.h"

typedef struct
{
    unsigned int nfiles; // ramdisk中的文件数
} initrd_header_t;

typedef struct
//文件格式信息
{
    unsigned char magic;     // Magic number
    unsigned char name[64];  // 文件名
    unsigned int offset;   // 文件在内存中的偏离起始位置的
			   //的大小，就是文件的地
    unsigned int length;   // 文件的长度
} initrd_file_header_t;

// 初始化initrd，location 是起始地址，由multiboot中的
//mods_addr给出，没错，就是那个神奇的multiboot
//返回root的inode
fs_node_t *initialise_initrd(unsigned int location);

#endif
