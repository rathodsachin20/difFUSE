#ifndef __global_h__
#define __global_h__

#define BLOCK_SIZE 64
#define NUM_INODES 15
#define FS_MIN_SIZE 4096
#define INODE_SIZE 32
#define INODE_NUM_DIRECT_BLOCKS 10
#define INODE_NUM_INDIRECT_ADDRS 4
#define FREE_BLOCKS_LIST_SIZE 4
#define FREE_INODES_LIST_SIZE 5
#define NUM_INODE_BLOCKS (NUM_INODES * INODE_SIZE) / BLOCK_SIZE + 1
#define MAX_NUM_FILE 32
#define MAX_FILE_NAME_LEN 32
#define is_bigendian() ( (*(char*)&i) == 0 )

#include <fcntl.h>
#include <stdio.h>
//#include <fuse.h>
#include <stdint.h>
#include <stdlib.h>

struct superblock{
    unsigned long size_fs;
    unsigned num_free_blocks;
    unsigned long list_free_blocks[FREE_BLOCKS_LIST_SIZE];
    int16_t index_next_free_block;
    //struct inode **list_free_inodes;
    unsigned long list_free_inodes[FREE_INODES_LIST_SIZE];
    int16_t index_next_free_inode;
    unsigned num_free_inodes; //Total num of free inodes
}superblock;

struct inode{
    //unsigned inode_number; //may not be needed
    int owner_id;
    int group_id;
    uint8_t type;
    int perms;
    float accessed;
    float file_modified;
    float inode_modified;
    // ? file_links;
    long int file_size;
    // ? long int size_partition;
    // ? bool file_is_locked;
    long int direct_blocks[INODE_NUM_DIRECT_BLOCKS];
    long int single_indirect_block;
    long int double_indirect_block;
    long int triple_indirect_block;
};

/* List of block ids to be stored in a block.
*  Can be used in Free blocks list, indirect pointers
*/
struct block_list{
    long list[BLOCK_SIZE/sizeof(long)];
};

struct directory{
    long int inode_num[MAX_NUM_FILE];
    char name[MAX_NUM_FILE][MAX_FILE_NAME_LEN];
}

#endif
