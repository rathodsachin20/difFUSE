#ifndef __layer2_h__
#define __layer2_h__

//Implements system calls for the file system

#include "layer1.h"


block_num get_file_block_num(int block_index, block_num inode_num, bool allocate);

//int add_inode_entry(const char* filepath, block_num file_inum){
void add_inode_entry(const char* filepath, block_num file_inum);

block_num get_parent_inode_num(const char* filepath);

block_num fs_namei(const char* filep);

int fs_unlink(const char* filepath);

int fs_create(const char *filepath, mode_t mode);

int fs_create_dir(const char *filepath, mode_t mode);

int fs_open(const char* filepath);

int fs_read(const char *filepath, char *buf, size_t count, off_t offset);

int fs_write(const char* filepath, long offset, const char* buffer, size_t size);

int fs_getattr(const char* filepath, struct stat* stbuf);

int fs_readdir(const char *filepath, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi);

int fs_mod_time(const char* path, const struct timespec tv[2]);

int fs_rmdir(const char* path);

int fs_rename(const char* oldp, const char* newp);

int fs_chmod(const char* path, mode_t mode);

int fs_chown(const char* path, uid_t uid, gid_t gid);

#endif /* __layer2_h__  */
