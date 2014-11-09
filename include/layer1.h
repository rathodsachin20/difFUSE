#ifndef __layer1_h__
#define __layer1_h__

#include "layer0.h"
#include "mkfs1.h"


long get_free_block_num(FILE* fp);

void free_inode(FILE* fp, long inumber);

void get_inode_struct(FILE* fp, long inode_num, struct inode* nodep);

void write_free_list_block(FILE* fp, long block_num, struct block_list* bl);

//void get_new_free_blocks_list(int ** newlist, long block_num, FILE* fp);
void update_free_blocks_list(struct superblock* sb, long block_num, FILE* fp);

long get_file_block_num(long offset, struct inode node, int* block_pos, FILE* fp);

void free_block(FILE* fp, long block_num);

long get_free_inode(FILE* fp, struct inode* nodep);

//void write_inode_struct(FILE* fp, long inode_num, struct inode* nodep);




#endif /* __layer1_h__  */
