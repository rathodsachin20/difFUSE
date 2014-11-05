#ifndef __fs_h__
#define __fs_h__

#include "fsutil.c"
#include "fs_utility.c"


//int fs_open();

//int fs_close();

//int fs_create(const char* filepath, perm_t perms);

int fs_write(const char* filepath, long offset, const char* buffer, long size, FILE* fp);

//int fs_read(const char* filepath, long offset, char* buffer, long size, FILE* fp);

long get_file_block_num(long offset, struct inode node, int* block_pos, FILE* fp);

#endif
