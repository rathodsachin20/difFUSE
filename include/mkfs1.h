#ifndef __mkfs_h__
#define __mkfs_h__

#include "layer0.h"

void mkfs(const char* filepath);
//void initialize_superblock(FILE* fp);
void initialize_superblock(int fd);
//unsigned long get_fs_size(FILE* fp);
unsigned long get_fs_size(int fd);
void initalize_inodes();
void write_inode(block_num inode_number, struct inode* inodep);
size_t write_block(const void* buffer, block_num num, int offset, int size);
void initialize_free_blocks(long size_fs);
void write_superblock();




#endif /* __mkfs_h__ */
