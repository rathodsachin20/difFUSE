#ifndef __layer1_h__
#define __layer1_h__

#include "layer0.h"
#include "mkfs1.h"


//block_num get_free_inode(FILE* fp, struct inode* nodep);
block_num allocate_inode(FILE* fp, struct inode* nodep);

void free_inode(FILE* fp, block_num inumber);

//void get_inode_struct(FILE* fp, block_num inode_num, struct inode* nodep);
void read_inode(FILE* fp, block_num inode_num, struct inode* nodep);

//void write_inode_struct(FILE* fp, block_num inode_num, struct inode* nodep);

//block_num get_free_block_num(FILE* fp);
block_num allocate_block(FILE* fp);

void free_block(FILE* fp, block_num block_no);

int read_block(void* buffer, block_num num, int offset, int size, FILE* fp);

void write_block(const void* buffer, block_num num, int offset, int size, FILE* fp);

void write_link_block(FILE* fp, block_num num, struct block_list* bl);

//void get_new_free_blocks_list(int ** newlist, block_num block_num, FILE* fp);
void update_free_blocks_list(struct superblock* sb, block_num block_no, FILE* fp);


block_num allocate_block_list(FILE* fp);

int read_block_list(void* bl, block_num bn, FILE* fp);

void write_block_list(const void* bl, block_num bn, FILE* fp);


#endif /* __layer1_h__  */
