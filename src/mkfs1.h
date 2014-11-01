#ifndef __mkfs_h__
#define __mkfs_h__

#include <fcntl.h>
#include <stdio.h>
#include <fuse.h>
#include <stdint.h>
#include "global.h"

void mkfs(const char* filepath);
void initialize_superblock(FILE* fp);
unsigned long get_fs_size(FILE* fp);
void initalize_inodes(FILE* fp);
void write_inode(FILE* fp, long inode_number, struct inode* inodep);
void initialize_free_blocks(long size_fs, FILE* fp);
void write_superblock();




#endif
