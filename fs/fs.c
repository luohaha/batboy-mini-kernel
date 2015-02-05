#include "fs.h"
fs_node_t *fs_root = 0; // The root of the filesystem.

unsigned int read_fs(fs_node_t *node, unsigned int offset, unsigned int size, unsigned char *buffer)
//读取索引节点node的文件数据，读取从offset位置开始，大小为size的数据
//将他们写到buffer中
{
    if (node->read != 0)
	//有读函数
        return node->read(node, offset, size, buffer);
    else
        return 0;
}

unsigned int write_fs(fs_node_t *node, unsigned int offset, unsigned int size, unsigned char *buffer)
//同理,将buffer数据写到，文件从offset开始，长度size的区域
{
    if (node->write != 0)
        return node->write(node, offset, size, buffer);
    else
        return 0;
}

void open_fs(fs_node_t *node, unsigned char read, unsigned char write)
//打开文件
{
    if (node->open != 0)
        return node->open(node);
}

void close_fs(fs_node_t *node)
{
    if (node->close != 0)
        return node->close(node);
}

struct dirent *readdir_fs(fs_node_t *node, unsigned int index)
//返回目录文件的所有目录项的起始地址
{
    if ( (node->flags&0x7) == FS_DIRECTORY &&
         node->readdir != 0 )
        return node->readdir(node, index);
    else
        return 0;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name)
//寻找目录文件下，名字为name的文件的inode项
{
    if ( (node->flags&0x7) == FS_DIRECTORY &&
         node->finddir != 0 )
        return node->finddir(node, name);
    else
        return 0;
}
