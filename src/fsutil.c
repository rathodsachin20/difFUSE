//#ifndef __fsutil_c__
//#define __fsutil_c__

#include "fsutil.h"

long get_free_block_num(FILE* fp){
    struct superblock* sb;
    sb = &superblock;
    int16_t index = sb->index_next_free_block;
    printf("Index:%d\n", index);
    long block_num = sb->list_free_blocks[index];

    if(block_num == 0){
        printf("File System is full!\n");
        return 0;
    }
    if(index != 0){
        sb->index_next_free_block -= 1;
    }
    else{ //List is now empty, refill it
        printf("List is now empty, refill it\n");
        // We can copy list into superblock so that superblock is locked for less time
        //long newlist[FREE_BLOCKS_LIST_SIZE];
        //get_new_free_blocks_list(&newlist, block_num, fp);
        update_free_blocks_list(sb, block_num, fp);
        block_num = sb->list_free_blocks[FREE_BLOCKS_LIST_SIZE-1];
        sb->index_next_free_block = FREE_BLOCKS_LIST_SIZE-2;
        //int i;
        //for(i=0; i<FREE_BLOCKS_LIST_SIZE; i++){
        //    sb->list_free_blocks[i] = newlist[i];
        //}
    }
    printf("Gave block num %ld", block_num);
    return block_num;
}


//
void write_free_list_block(FILE* fp, long block_num, struct block_list* bl){
    fseek(fp, BLOCK_SIZE * block_num, SEEK_SET);
    fwrite(bl, sizeof(struct block_list), 1, fp);
    if(ferror(fp))
        perror("Error ");
    fflush(fp);
}

//void get_new_free_blocks_list(int ** newlist, long block_num, FILE* fp){
// Need to handle scenario when file is full
void update_free_blocks_list(struct superblock* sb, long block_num, FILE* fp){
    int i;
    struct block_list bl;
    long curr_block = block_num;
    int curr_index = FREE_BLOCKS_LIST_SIZE-1;
    int num_entries = BLOCK_SIZE/sizeof(long);
    long* list = sb->list_free_blocks;
    while(curr_block){
        printf("curr block num=%ld\n", curr_block);
        fseek(fp, BLOCK_SIZE * curr_block, SEEK_SET);
        fread(&bl, sizeof(struct block_list), 1, fp);

        for(i = num_entries-1; i >= 0; i--){
            if(curr_index == 0){
                list[0] = curr_block;
                //break;
                write_free_list_block(fp, curr_block, &bl);
                return;
            }
            else
            if(curr_index == 1  &&  i == 0){
                list[1] = curr_block;
                list[0] = bl.list[i];
                //break;
                write_free_list_block(fp, curr_block, &bl);
                return;
            }
            else
            if(bl.list[i]){
                if(i != 0){
                    list[curr_index] = bl.list[i];
                    bl.list[i] = 0;
                    curr_index--;
                }
                else{
                    list[curr_index] = curr_block;
                    curr_block = bl.list[0];
                    //printf("currblock=%ld,old=%ld\n",curr_block, list[curr_index]);
                    //if(curr_block>155){
                    //    return;
                    //}
                    //if(curr_block == 0 || curr_block==list[curr_index]){ //Last block of list
                    //    for( ; curr_index>=0; curr_index--){
                    //        list[curr_index] = 0;
                    //    }
                    //    write_free_list_block(fp, curr_block, &bl);
                    //    return;
                    //}
                    //bl.list[0] = 0; //not needed
                    curr_index--;
                }
            }
        }


        //curr_block = bl.list[0];
        printf("next block is:%ld\n", curr_block);

    }
    sb->list_free_blocks[0] = 0; // No more free blocks
    //(*newlist)[0] = 0;
    return;

}

long get_free_block_offset(FILE* fp){
    long block_num = get_free_block_num(fp);
    return BLOCK_SIZE * block_num;
}



void free_block(FILE*fp, long block_num){
    printf("<------FREEING BLOCK NO %ld---->\n", block_num);
    if(block_num < 1 + NUM_INODE_BLOCKS){
        return;
    }
    struct superblock * sb = &superblock;
    long index = sb->index_next_free_block;
    if(index < FREE_BLOCKS_LIST_SIZE-1){
        index++;
        sb->list_free_blocks[index] = block_num;
        sb->index_next_free_block = index;
    }
    else{// Change link from sb[0]->x to sb[0]->block_num->x
        int i;
        struct block_list bl;
        bl.list[0] = sb->list_free_blocks[0];
        sb->list_free_blocks[0] = block_num;
        for(i=1; i<BLOCK_SIZE/sizeof(long); i++){
            bl.list[i] = 0L;
        }
        write_free_list_block(fp, block_num, &bl);
    }
}





/*
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
  */  
 /*
        for(i=num_entries-1; i>=0; i--){
            printf(" entry at %d.is.%ld..for %d.\n", i, bl.list[i], curr_index);
            if(i==0){
                
                if(curr_index != 0){
                    sb->list_free_blocks[curr_index] = curr_block;
                }
                else{
                    sb->list_free_blocks[0] = bl.list[0];
                }
                curr_block = bl.list[0];
            }
            else 
            if(bl.list[i]){
                //(*newlist)[curr_index] = bl->list[i];
                sb->list_free_blocks[curr_index] = bl.list[i];
                //printf("Got as %ld at %d", bl->list[i], curr_index);
                bl.list[i] = 0;
                curr_index--;
                if(curr_index==0){ //Last element to be filled
                    //(*newlist)[0] = curr_block;
                    sb->list_free_blocks[0] = curr_block;
                    return;
                }
            }
        }
        // Moving to next block
        //if(curr_index==1){
        if(i==0){
            //(*newlist)[1] = curr_block;
            sb->list_free_blocks[1] = curr_block;
            printf("Entry for 1 is %ld\n", curr_block);
            //(*newlist)[0] = bl->list[0];
            sb->list_free_blocks[0] = bl.list[0];
            printf("Entry for 0 is %ld\n", bl.list[0]);
            return;
        }

         */

//#endif
