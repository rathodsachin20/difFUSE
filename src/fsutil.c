//#ifndef __fsutil_c__
//#define __fsutil_c__

#include "fsutil.h"

long get_free_block_num(FILE* fp){
    struct superblock* sb;
    sb = &superblock;
    int16_t index = sb->index_next_free_block;
    printf("Index:%d\n", index);
    long block_num = sb->list_free_blocks[index];
    if(block_num==0){
        printf("File System is full!\n");
        return 0;
    }
    index--;
    printf("Index:%d\n", index);
    if(index<0){ //List is now empty, refill it
        printf("List is now empty, refill it\n");
        int *newlist;
        newlist = (int*) calloc(FREE_BLOCKS_LIST_SIZE, sizeof(long));
        get_new_free_blocks_list(&newlist, block_num, fp);
        int i;
        for(i=0; i<FREE_BLOCKS_LIST_SIZE; i++){
            sb->list_free_blocks[i] = newlist[i];
        }
    }
    return block_num;
}

void get_new_free_blocks_list(int ** newlist, long block_num, FILE* fp){
    int i;
    struct block_list* bl;
    long new_block_num = block_num;
    int current_index = FREE_BLOCKS_LIST_SIZE-1;
    int num_entries = BLOCK_SIZE/sizeof(long);
    while(new_block_num){
        printf("block num=%ld\n", block_num);
        fseek(fp, BLOCK_SIZE * new_block_num, SEEK_SET);
        fread(bl, sizeof(struct block_list), 1, fp);

        for(i=num_entries-1; i>=0; i--){
            if(bl->list[i]){
                (*newlist)[current_index] = bl->list[i];
                bl->list[i] = 0;
                current_index--;
                if(current_index==0){
                    (*newlist)[0] = new_block_num;
                    return;
                }
            }
        }
        // Moving to next block
        if(current_index==1){
            (*newlist)[1] = new_block_num;
            (*newlist)[0] = bl->list[0];
            return;
        }
        new_block_num = bl->list[0];

    }
    (*newlist)[0] = 0;
    return;

}

long get_free_block_offset(FILE* fp){
    long block_num = get_free_block_num(fp);
    return BLOCK_SIZE * block_num;
}

//implementing allocation of free inode...
struct inode* alloc_inode(FILE *fp){
    struct superblock *sb = &superblock;
    struct inode* temp = NULL;
    int16_t index = sb->index_next_free_inode;
    if(index != 0){
	//allocate from free inode list in sb
	inode_num = sb->list_free_inodes[index];
	if( get_free_inode(fp,inode_num,&temp) == 0 ){
	    printf("Could not read inode\n");
	    }
	//initialize inode
    }
    else{
	//scan for new inode. refill the list.
    }
}

long int get_free_inode(FILE * fp, long int inode_num, struct node* inodep){
    long int num_inode_per_block = BLOCK_SIZE/INODE_SIZE;
    long int block_num = ((inode_num-1) / num_inode_per_block) + 1;
    long int offset = ((inode_num-1) % num_inode_per_block) * INODE_SIZE;
    long int pos = (block_num * BLOCK_SIZE) + offset;
    fseek(fp, pos, SEEK_SET);
    fread(inodep, sizeof(struct inode), 1, fp);
    }
    

//#endif
