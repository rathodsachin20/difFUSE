#ifndef __layer2_h__
#define __layer2_h__

//Implements system calls for the file system

#include "layer1.h"


//int add_inode_entry(const char* filepath, long file_inum, FILE* fp){
void add_inode_entry(const char* filepath, long file_inum, FILE* fp);

long get_parent_inode_num(const char* filepath, FILE* fp);

long int fs_namei(FILE* fp, const char* filep);

int fs_create(const char *filepath, mode_t mode, struct fuse_file_info * ffi, FILE* fp);

int fs_create_dir(const char *filepath, mode_t mode, struct fuse_file_info * ffi, FILE* fp);

int fs_open(const char* filepath, struct fuse_file_info* ffi, FILE* fp);

int fs_read(const char *filepath, char *buf, size_t count, off_t offset, struct fuse_file_info * ffi, FILE* fp);

int fs_write(const char* filepath, long offset, const char* buffer, long size, FILE* fp);

#endif /* __layer2_h__  */
