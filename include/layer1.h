#ifndef __layer1_h__
#define __layer1_h__

#include "layer0.h"
#include "mkfs1.h"


//block_num get_free_inode(struct inode* nodep);
block_num allocate_inode(struct inode* nodep);

void free_inode(block_num inumber);

//void get_inode_struct(block_num inode_num, struct inode* nodep);
void read_inode(block_num inode_num, struct inode* nodep);

//void write_inode_struct(block_num inode_num, struct inode* nodep);

//block_num get_free_block_num();
block_num allocate_block();

void free_block(block_num block_no);

int read_block(void* buffer, block_num num, int offset, int size);

//void write_block(const void* buffer, block_num num, int offset, int size);

void write_link_block(block_num num, struct block_list* bl);

//void get_new_free_blocks_list(int ** newlist, block_num block_num);
void update_free_blocks_list(struct superblock* sb, block_num block_no);


block_num allocate_block_list();

int read_block_list(void* bl, block_num bn);

void write_block_list(const void* bl, block_num bn);

void initialize_dir_block(block_num block_no);


#endif /* __layer1_h__  */
