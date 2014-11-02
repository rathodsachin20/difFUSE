//#ifndef __fsutil_c__
//#define __fsutil_c__

#include "fsutil.h"

long get_free_block_num(FILE* fp){
    struct superblock* sb;
    sb = &superblock;
    int16_t index = sb->index_next_free_block;
    printf("Index:%d\n", index);
    if(index <0){
        printf("File System is full!\n");
        return 0;
    }
    long block_num = sb->list_free_blocks[index];
    if(block_num==0 ){
        printf("File System is full!\n");
        return 0;
    }
    sb->index_next_free_block -= 1;
    index--;
    printf("new Index:%d\n", index);
    if(index<0){ //List is now empty, refill it
        printf("List is now empty, refill it\n");
        // We can copy list into superblock so that superblock is locked for less time
        //long newlist[FREE_BLOCKS_LIST_SIZE];
        //get_new_free_blocks_list(&newlist, block_num, fp);
        get_new_free_blocks_list(sb, block_num, fp);
        sb->index_next_free_block = FREE_BLOCKS_LIST_SIZE-1;
        //int i;
        //for(i=0; i<FREE_BLOCKS_LIST_SIZE; i++){
        //    sb->list_free_blocks[i] = newlist[i];
        //}
    }
    printf("Gave block num %ld", block_num);
    return block_num;
}

//void get_new_free_blocks_list(int ** newlist, long block_num, FILE* fp){
void get_new_free_blocks_list(struct superblock* sb, long block_num, FILE* fp){
    int i;
    struct block_list bl;
    long curr_block_num = block_num;
    int current_index = FREE_BLOCKS_LIST_SIZE-1;
    int num_entries = BLOCK_SIZE/sizeof(long);
    while(curr_block_num){
        printf("curr block num=%ld\n", curr_block_num);
        fseek(fp, BLOCK_SIZE * curr_block_num, SEEK_SET);
        fread(&bl, sizeof(struct block_list), 1, fp);
        for(i=num_entries-1; i>=0; i--){
            printf(" entry at %d.is.%ld..for %d.\n", i, bl.list[i], current_index);
            if(i==0){
                
                if(current_index != 0){
                    sb->list_free_blocks[current_index] = curr_block_num;
                }
                else{
                    sb->list_free_blocks[0] = bl.list[0];
                }
                curr_block_num = bl.list[0];
            }
            else 
            if(bl.list[i]){
                //(*newlist)[current_index] = bl->list[i];
                sb->list_free_blocks[current_index] = bl.list[i];
                //printf("Got as %ld at %d", bl->list[i], current_index);
                bl.list[i] = 0;
                current_index--;
                if(current_index==0){ //Last element to be filled
                    //(*newlist)[0] = curr_block_num;
                    sb->list_free_blocks[0] = curr_block_num;
                    return;
                }
            }
        }
        // Moving to next block
        //if(current_index==1){
        if(i==0){
            //(*newlist)[1] = curr_block_num;
            sb->list_free_blocks[1] = curr_block_num;
            printf("Entry for 1 is %ld\n", curr_block_num);
            //(*newlist)[0] = bl->list[0];
            sb->list_free_blocks[0] = bl.list[0];
            printf("Entry for 0 is %ld\n", bl.list[0]);
            return;
        }
        //curr_block_num = bl.list[0];
        printf("next block is:%ld\n", curr_block_num);

    }
    sb->list_free_blocks[0] = 0; // No more free blocks
    //(*newlist)[0] = 0;
    return;

}

long get_free_block_offset(FILE* fp){
    long block_num = get_free_block_num(fp);
    return BLOCK_SIZE * block_num;
}


//#endif
