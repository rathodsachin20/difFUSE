#ifndef __fsutil_h__
#define __fsutil_h__

#include "global.h"
#include "mkfs1.h"


long get_free_block_num(FILE* fp);

//void get_new_free_blocks_list(int ** newlist, long block_num, FILE* fp);
void update_free_blocks_list(struct superblock* sb, long block_num, FILE* fp);

long get_free_block_offset();

void free_block(FILE* fp, long block_num);

#endif
