#ifndef __mkfs_h__
#define __mkfs_h__

#include "layer0.h"

void mkfs(const char* filepath);
void initialize_superblock(FILE* fp);
unsigned long get_fs_size(FILE* fp);
void initalize_inodes(FILE* fp);
void write_inode(FILE* fp, block_num inode_number, struct inode* inodep);
void write_block(const void* buffer, block_num num, int offset, int size, FILE* fp);
void initialize_free_blocks(long size_fs, FILE* fp);
void write_superblock();




#endif /* __mkfs_h__ */
