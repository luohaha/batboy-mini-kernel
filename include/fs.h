#ifndef FS_H
#define FS_H

#include "common.h"

#define FS_FILE        0x01//普通文件
#define FS_DIRECTORY   0x02//目录文件
#define FS_CHARDEVICE  0x03//字符型文件
#define FS_BLOCKDEVICE 0x04//块文件，按数据块读写
#define FS_PIPE        0x05//管道文件
#define FS_SYMLINK     0x06//符号链接
#define FS_MOUNTPOINT  0x08 // Is the file an active mountpoint?

struct fs_node;

//定义指向函数的指针，对应不同的文件系统，有不同的函数，使用这种方式可以
//统一接口
typedef unsigned int (*read_type_t)(struct fs_node*,unsigned int,unsigned int,unsigned char*);//读文件
typedef unsigned int (*write_type_t)(struct fs_node*,unsigned int,unsigned int,unsigned char*);//写文件
typedef void (*open_type_t)(struct fs_node*);//打开文件，然后可以接下其他动作
typedef void (*close_type_t)(struct fs_node*);//关闭文件
typedef struct dirent * (*readdir_type_t)(struct fs_node*,unsigned int);
//获取目录文件信息,返回目录项指针，因为有很多的目录项（每个文件对应一个）
typedef struct fs_node * (*finddir_type_t)(struct fs_node*,char *name);
//找到文件
typedef struct fs_node
{
    char name[128];     // 文件名
    unsigned int mask;        // 权限掩码，读写和执行
    unsigned int uid;         // 用户id，user id
    unsigned int gid;         // 组id，group id
    unsigned int flags;       // 文件类型
    unsigned int inode;       //文件id，唯一，用来表示不同的文件
    unsigned int length;      // 文件大小（byte）
    unsigned int impl;        // An implementation-defined number.
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    struct fs_node *ptr; // 有符号链接时存在
} fs_node_t;

struct dirent//目录项
//每个目录项对应一个名字和一个inode号，通过查找目录项，找到对应名字的文件的
//inode号，然后再去查找inode，才能接下来的操作
{
    char name[128]; // Filename.
    unsigned int ino;     // Inode number. Required by POSIX.
};

extern fs_node_t *fs_root; // The root of the filesystem.

// Standard read/write/open/close functions.
//定义虚拟文件操作函数，屏蔽地下的实际的文件操作函数
unsigned int read_fs(fs_node_t *node, unsigned int offset, unsigned int size, unsigned char *buffer);
unsigned int write_fs(fs_node_t *node, unsigned int offset, unsigned int size, unsigned char *buffer);
void open_fs(fs_node_t *node, unsigned char read, unsigned char write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, unsigned int index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);

#endif
