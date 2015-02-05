#include "initrd.h"
#include "memory.h"

initrd_header_t *initrd_header;     // The header.
initrd_file_header_t *file_headers; // 指向文件们
fs_node_t *initrd_root;             // root文件夹
fs_node_t *initrd_dev;              // 挂载点文件夹
fs_node_t *root_nodes;              // 文件数组的头地址
int nroot_nodes;                    // 文件个数

struct dirent dirent;

static unsigned int initrd_read(fs_node_t *node, unsigned int offset, unsigned int size, unsigned char *buffer)
{
    initrd_file_header_t header = file_headers[node->inode];
	//找到对应inode号的文件头信息
    if (offset > header.length)
        return 0;
    if (offset+size > header.length)
	//读取的位置不能超过文件的范围
        size = header.length-offset;
    memcpy(buffer, (unsigned char*) (header.offset+offset), size);
	//header.offset是文件的起始地址
    return size;
}

static struct dirent *initrd_readdir(fs_node_t *node, unsigned int index)
{
    if (node == initrd_root && index == 0)
    //创建根节点下/dev目录文件的目录项
    {
      strcpy(dirent.name, "dev");
      dirent.name[3] = 0;
      dirent.ino = 0;
      return &dirent;
    }

    if (index-1 >= nroot_nodes)
	//超过了文件数，没有indoe号为index的文件
        return 0;
    //新建一个目录项，将找到的文件的信息存进去
    strcpy(dirent.name, root_nodes[index-1].name);
    dirent.name[strlen(root_nodes[index-1].name)] = 0;
    dirent.ino = root_nodes[index-1].inode;
    return &dirent;
}

static fs_node_t *initrd_finddir(fs_node_t *node, char *name)
{
	//如果是找/dev的话
    if (node == initrd_root &&
        !strcmp(name, "dev") )
        return initrd_dev;

    int i;
	//找普通文件
    for (i = 0; i < nroot_nodes; i++)
        if (!strcmp(name, root_nodes[i].name))
            return &root_nodes[i];
    return 0;
}

fs_node_t *initialise_initrd(unsigned int location)
{
    /*
	因为在创建的initrd.img文件中，数据是按以下
	方式创建的：
	1.initrd_header_t,致命有多少文件
	2.initrd_file_header_t,文件的信息文件
	  从这个文件中，可以读出包括文件实际位置在内的信息
	3.文件实在的数据
	所以在一下的初始化中，按照multiboot指出的起始位置,
	可以依次读出相关信息
	*/
    initrd_header = (initrd_header_t *)location;
    file_headers = (initrd_file_header_t *) (location+sizeof(initrd_header_t));

    // Initialise the root directory.
    initrd_root = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    strcpy(initrd_root->name, "initrd");
    initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode = initrd_root->length = 0;
    initrd_root->flags = FS_DIRECTORY;
    initrd_root->read = 0;
    initrd_root->write = 0;
    initrd_root->open = 0;
    initrd_root->close = 0;
    initrd_root->readdir = &initrd_readdir;
    initrd_root->finddir = &initrd_finddir;
    initrd_root->ptr = 0;
    initrd_root->impl = 0;

    // Initialise the /dev directory (required!)
    initrd_dev = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    strcpy(initrd_dev->name, "dev");
    initrd_dev->mask = initrd_dev->uid = initrd_dev->gid = initrd_dev->inode = initrd_dev->length = 0;
    initrd_dev->flags = FS_DIRECTORY;
    initrd_dev->read = 0;
    initrd_dev->write = 0;
    initrd_dev->open = 0;
    initrd_dev->close = 0;
    initrd_dev->readdir = &initrd_readdir;
    initrd_dev->finddir = &initrd_finddir;
    initrd_dev->ptr = 0;
    initrd_dev->impl = 0;

    root_nodes = (fs_node_t*)kmalloc(sizeof(fs_node_t) * initrd_header->nfiles);
    nroot_nodes = initrd_header->nfiles;

    // For every file...
    int i;
    for (i = 0; i < initrd_header->nfiles; i++)
    {
        // Edit the file's header - currently it holds the file offset
        // relative to the start of the ramdisk. We want it relative to the start
        // of memory.
        file_headers[i].offset += location;
        // Create a new file node.
        strcpy(root_nodes[i].name, &file_headers[i].name);
        root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
        root_nodes[i].length = file_headers[i].length;
        root_nodes[i].inode = i;
        root_nodes[i].flags = FS_FILE;
        root_nodes[i].read = &initrd_read;
        root_nodes[i].write = 0;
        root_nodes[i].readdir = 0;
        root_nodes[i].finddir = 0;
        root_nodes[i].open = 0;
        root_nodes[i].close = 0;
        root_nodes[i].impl = 0;
    }
    return initrd_root;
}
