/*
	存储multiboot的信息,方便以后使用
*/

#ifndef MULTIBOOT_H
#define MULTIBOOT_H
#include "common.h"

typedef struct multiboot_struct{
	unsigned int flags;			// Multiboot 的版本信息
	/** 
	 * 从 BIOS 获知的可用内存
	 *
	 * mem_lower和mem_upper分别指出了低端和高端内存的大小，单位是K。
	 * 低端内存的首地址是0，高端内存的首地址是1M。
	 * 低端内存的最大可能值是640K。
	 * 高端内存的最大可能值是最大值减去1M。但并不保证是这个值。
	 */
	unsigned int mem_lower;
	unsigned int mem_upper;

	unsigned int boot_device;		// 指出引导程序从哪个BIOS磁盘设备载入的OS映像
	unsigned int cmdline;		// 内核命令行
	unsigned int mods_count;		// boot 模块列表
	unsigned int mods_addr;
	
	/**
	 * ELF 格式内核映像的section头表。
	 * 包括每项的大小、一共有几项以及作为名字索引的字符串表。
	 */
	unsigned int num;
	unsigned int size;
	unsigned int addr;
	unsigned int shndx;

	/**
	 * 以下两项指出保存由BIOS提供的内存分布的缓冲区的地址和长度
	 * mmap_addr是缓冲区的地址，mmap_length是缓冲区的总大小
	 * 缓冲区由一个或者多个下面的 mmap_entry_t 组成
	 */
	unsigned int mmap_length;		
	unsigned int mmap_addr;
	
	unsigned int drives_length; 	// 指出第一个驱动器结构的物理地址	
	unsigned int drives_addr; 		// 指出第一个驱动器这个结构的大小
	unsigned int config_table; 		// ROM 配置表
	unsigned int boot_loader_name; 	// boot loader 的名字
	unsigned int apm_table; 	    	// APM 表
	unsigned int vbe_control_info;
	unsigned int vbe_mode_info;
	unsigned int vbe_mode;
	unsigned int vbe_interface_seg;
	unsigned int vbe_interface_off;
	unsigned int vbe_interface_len;
} __attribute__((packed)) multiboot_struct;

/**
 * size是相关结构的大小，单位是字节，它可能大于最小值20
 * base_addr_low是启动地址的低32位，base_addr_high是高32位，启动地址总共有64位
 * length_low是内存区域大小的低32位，length_high是内存区域大小的高32位，总共是64位
 * type是相应地址区间的类型，1代表可用RAM，所有其它的值代表保留区域
 */
typedef struct mmap_entry_t {
	unsigned int size; 		// 留意 size 是不含 size 自身变量的大小
	//因为可能要包括内存外的地址,所以base_addr和length都为64位
	unsigned int base_addr_low;
	unsigned int base_addr_high;
	unsigned int length_low;
	unsigned int length_high;
	//type 是指内存是位于ram中,还是其他地方
	unsigned int type;
} __attribute__((packed)) mmap_entry_t;
//使用extern是因为这个变量整个工程中,只能有一个,
//如果不加,每个链接这个头文件的c程序都有有自己的multiboot
extern multiboot_struct *glb_mboot_ptr_tmp;
extern multiboot_struct *glb_mboot_ptr;
#endif
