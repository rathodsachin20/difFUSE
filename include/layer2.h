#ifndef __layer2_h__
#define __layer2_h__

//Implements system calls for the file system

#include "layer1.h"


block_num get_file_block_num(int block_index, block_num inode_num, bool allocate, FILE* fp);

//int add_inode_entry(const char* filepath, block_num file_inum, FILE* fp){
void add_inode_entry(const char* filepath, block_num file_inum, FILE* fp);

block_num get_parent_inode_num(const char* filepath, FILE* fp);

block_num fs_namei(FILE* fp, const char* filep);

int fs_create(const char *filepath, mode_t mode, FILE* fp);

int fs_create_dir(const char *filepath, mode_t mode, FILE* fp);

int fs_open(const char* filepath, FILE* fp);

int fs_read(const char *filepath, char *buf, size_t count, off_t offset, FILE* fp);

int fs_write(const char* filepath, long offset, const char* buffer, long size, FILE* fp);

int fs_getattr(const char* filepath, struct stat* stbuf, FILE* fp);

int fs_readdir(const char *filepath, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi, FILE* fp);


#endif /* __layer2_h__  */
