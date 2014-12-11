#ifndef __global_h__
#define __global_h__

#define FUSE_USE_VERSION 26

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <fuse.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define BLOCK_SIZE 4096
#define NUM_INODES 20024
#define FS_MIN_SIZE 4096
#define INODE_SIZE 256
#define INODE_NUM_DIRECT_BLOCKS 10
#define INODE_NUM_INDIRECT_ADDRS 4
#define FREE_BLOCKS_LIST_SIZE 256
#define FREE_INODES_LIST_SIZE 128
#define NUM_INODE_BLOCKS (NUM_INODES * INODE_SIZE) / BLOCK_SIZE + 1
//#define MAX_NUM_FILE 100
#define MAX_FILE_NAME_LEN 56  //NAMEI_ENTRY_SIZE-sizeof(blocknum)
#define NAMEI_ENTRY_SIZE 64 // block size should be multiple of this
//#define is_bigendian() ( (*(char*)&i) == 0 )

int fs_desc;

typedef unsigned long block_num;
typedef int8_t bool;
#define true 1
#define false 0

struct superblock{
    unsigned long size_fs;
    
    block_num num_free_blocks;
    block_num list_free_blocks[FREE_BLOCKS_LIST_SIZE];
    int16_t index_next_free_block;
    
    block_num num_free_inodes;
    block_num list_free_inodes[FREE_INODES_LIST_SIZE];
    int16_t index_next_free_inode;
    
   // bool is_locked;
}superblock;

struct inode{
    //unsigned inode_number; //may not be needed
    uint16_t owner_id;
    uint16_t group_id;
    //uint8_t type;
    //int16_t perms;
    mode_t mode;
    time_t accessed;
    time_t file_modified;
    time_t inode_modified;
    uint16_t links_count;
    unsigned long file_size;
    // ? long int size_partition;
    // ? bool file_is_locked;
    block_num direct_blocks[INODE_NUM_DIRECT_BLOCKS];
    block_num single_indirect_block;
    block_num double_indirect_block;
    block_num triple_indirect_block;
    long last_filled_block_index;
};

/* List of block ids to be stored in a block.
*  Can be used in Free blocks list, indirect pointers
*/
struct block_list{
    block_num list[BLOCK_SIZE/sizeof(block_num)];
};

struct directory{
    block_num inode_num[BLOCK_SIZE/NAMEI_ENTRY_SIZE];
    char name[BLOCK_SIZE/NAMEI_ENTRY_SIZE][NAMEI_ENTRY_SIZE - sizeof(block_num)];
};

#endif
