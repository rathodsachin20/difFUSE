#ifndef __global_h__
#define __global_h__

#include <fcntl.h>
#include <stdio.h>
#include <fuse.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#define BLOCK_SIZE 512
#define NUM_INODES 15
#define FS_MIN_SIZE 4096
#define INODE_SIZE 128
#define INODE_NUM_DIRECT_BLOCKS 10
#define INODE_NUM_INDIRECT_ADDRS 4
#define FREE_BLOCKS_LIST_SIZE 4
#define FREE_INODES_LIST_SIZE 20
#define NUM_INODE_BLOCKS (NUM_INODES * INODE_SIZE) / BLOCK_SIZE + 1
#define MAX_NUM_FILE 10 
#define MAX_FILE_NAME_LEN 32
//#define is_bigendian() ( (*(char*)&i) == 0 )

typedef unsigned long block_num;
typedef int8_t bool;
#define true 1
#define false 1

struct superblock{
    unsigned long size_fs;
    
    block_num num_free_blocks;
    block_num list_free_blocks[FREE_BLOCKS_LIST_SIZE];
    int16_t index_next_free_block;
    
    block_num num_free_inodes;
    block_num list_free_inodes[FREE_INODES_LIST_SIZE];
    int16_t index_next_free_inode;
    
    bool is_locked;
}superblock;

struct inode{
    //unsigned inode_number; //may not be needed
    int owner_id;
    int group_id;
    int type;
    int16_t perms;
    float accessed;
    float file_modified;
    float inode_modified;
    uint16_t links_count;
    unsigned long file_size;
    // ? long int size_partition;
    // ? bool file_is_locked;
    block_num direct_blocks[INODE_NUM_DIRECT_BLOCKS];
    block_num single_indirect_block;
    block_num double_indirect_block;
    block_num triple_indirect_block;
};

/* List of block ids to be stored in a block.
*  Can be used in Free blocks list, indirect pointers
*/
struct block_list{
    block_num list[BLOCK_SIZE/sizeof(block_num)];
};

struct directory{
    block_num inode_num[MAX_NUM_FILE];
    char name[MAX_NUM_FILE][MAX_FILE_NAME_LEN];
};

#endif
