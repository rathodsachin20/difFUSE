#ifndef __layer1_h__
#define __layer1_h__

#include "layer0.h"
#include "mkfs1.h"


//long get_free_inode(FILE* fp, struct inode* nodep);
long allocate_inode(FILE* fp, struct inode* nodep);

void free_inode(FILE* fp, long inumber);

//void get_inode_struct(FILE* fp, long inode_num, struct inode* nodep);
void read_inode(FILE* fp, long inode_num, struct inode* nodep);

//void write_inode_struct(FILE* fp, long inode_num, struct inode* nodep);

//long get_free_block_num(FILE* fp);
long allocate_block(FILE* fp);

void free_block(FILE* fp, long block_num);

void read_block(void* buffer, block_num num, int offset, int size, FILE* fp);

void write_block(const void* buffer, block_num num, int offset, int size, FILE* fp);

void write_link_block(FILE* fp, block_num num, struct block_list* bl);

//void get_new_free_blocks_list(int ** newlist, long block_num, FILE* fp);
void update_free_blocks_list(struct superblock* sb, long block_num, FILE* fp);





#endif /* __layer1_h__  */
