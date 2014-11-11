#include "layer1.h"

/* Get a new inode struct
*/
block_num allocate_inode(FILE* fp, struct inode* nodep){
    struct superblock* sb = &superblock;
    block_num inode_num;
    int16_t index = sb->index_next_free_inode;
    struct inode curr_inode;
    
    //printf("INDEX=%d..", index);
    inode_num = sb->list_free_inodes[index];
    if(inode_num==0){
        printf("NO MORE INODES AVAILABLE!\t");
        return 0;
    }
    if(index > 0){
        // return node at index, update index
        sb->index_next_free_inode -= 1;
    }
    else{ //index=0, return this node, update list in superblock, update index
        //inode_num = sb->list_free_inodes[0];
        block_num curr_inode_num = inode_num + 1;
        if(curr_inode_num == NUM_INODES){ // This is last free inode
            printf("GIVING LAST INODE! :(\n");
            sb->list_free_inodes[index] = 0;
        }
        else if(curr_inode_num > NUM_INODES){ //
            printf("NO MORE INODES AVAILABLE!\t");
            return 0;
        }
        else{
            sb->index_next_free_inode = FREE_INODES_LIST_SIZE-1;
            int16_t curr_index = FREE_INODES_LIST_SIZE-1;
            // Get new inodes
            while(curr_inode_num < NUM_INODES){
                //printf("Curr_inode_num=%ld\n", curr_inode_num);
                if(curr_index < 0)
                    break;
                read_inode(fp, curr_inode_num, &curr_inode);
                if(curr_inode.type == 0){
                    //printf("Free inode_num=%ld\n", curr_inode_num);
                    sb->list_free_inodes[curr_index] = curr_inode_num;
                    curr_index--;
                }
                curr_inode_num++;
                if(curr_inode_num == NUM_INODES){
                    //printf("Last inode added. curr_index=%d\n",curr_index);
                    int i;
                    for(; curr_index>=0; curr_index--){
                        sb->list_free_inodes[curr_index] = 0;
                    }
                }
            }
        }
    }
    read_inode(fp, inode_num, nodep);
    nodep->type = 1;
    write_inode(fp, inode_num, nodep);
    printf("Giving inode no. %ld\t", inode_num);
    sb->num_free_inodes -= 1;
    return inode_num;
}

/* Free an inode in use. Add it to free inodes list.
*/
void free_inode(FILE* fp, block_num inumber){
    if(inumber > 1 + NUM_INODES || inumber <1){
        return;
    }
    // Make type=0 and write to disk - NO! it's duplicate
    printf("<----------- FREEING INODE NO %ld ------------->\n", inumber);
    //struct inode node;
    //node.type = 0;
    //write_inode(fp, inumber, &node);
    
    // Update cached free inodes list in superblock
    struct superblock * sb = &superblock;
    int16_t index = sb->index_next_free_inode;
    sb->num_free_inodes += 1;
    if(index < FREE_INODES_LIST_SIZE-1){// List is not full
        sb->list_free_inodes[index+1] = inumber;
        sb->index_next_free_inode += 1;
    }
    else{// List is full, replace item [0] if it is larger than inumber
        if(inumber < sb->list_free_inodes[0]){
            sb->list_free_inodes[0] = inumber;
        }
        else{ // inode not entered in cached list, need to write back to disk
            struct inode node;
            node.type = 0;
            write_inode(fp, inumber, &node);
        }
    }
    
}

/* Read contents of inode into inode struct
*/
void read_inode(FILE* fp, block_num inode_num, struct inode* nodep){
    block_num block_no = ((inode_num - 1) / (BLOCK_SIZE/INODE_SIZE)) + 1;
    int offset = ((inode_num - 1) % (BLOCK_SIZE/INODE_SIZE)) * INODE_SIZE;
    read_block((void*) nodep, block_no, offset, sizeof(struct inode), fp);
}

block_num allocate_block(FILE* fp){
    struct superblock* sb;
    sb = &superblock;
    int16_t index = sb->index_next_free_block;
    printf("Index:%d\n", index);
    block_num block_no = sb->list_free_blocks[index];
    
    if(block_no == 0){
        printf("File System is full!\n");
        return 0;
    }
    if(index != 0){
        sb->index_next_free_block -= 1;
    }
    else{ //List is now empty, refill it
        printf("List is now empty, refill it\n");
        // We can copy list into superblock so that superblock is locked for less time
        //block_num newlist[FREE_BLOCKS_LIST_SIZE];
        //get_new_free_blocks_list(&newlist, block_no, fp);
        update_free_blocks_list(sb, block_no, fp);
        block_no = sb->list_free_blocks[FREE_BLOCKS_LIST_SIZE-1];
        sb->index_next_free_block = FREE_BLOCKS_LIST_SIZE-2;
        //int i;
        //for(i=0; i<FREE_BLOCKS_LIST_SIZE; i++){
        //    sb->list_free_blocks[i] = newlist[i];
        //}
    }
    printf("Gave block num %ld", block_no);
    return block_no;
}

/* Free a block in use. Add it to free blocks list.
*/
void free_block(FILE* fp, block_num block_no){
    printf("<------FREEING BLOCK NO %ld---->\n", block_no);
    if(block_no < 1 + NUM_INODE_BLOCKS){
        return;
    }
    struct superblock * sb = &superblock;
    int16_t index = sb->index_next_free_block;
    if(index < FREE_BLOCKS_LIST_SIZE-1){
        index++;
        sb->list_free_blocks[index] = block_no;
        sb->index_next_free_block = index;
    }
    else{// Change link from sb[0]->x to sb[0]->block_no->x
        int i;
        struct block_list bl;
        bl.list[0] = sb->list_free_blocks[0];
        sb->list_free_blocks[0] = block_no;
        for(i=1; i<BLOCK_SIZE/sizeof(block_num); i++){
            bl.list[i] = 0L;
        }
        write_link_block(fp, block_no, &bl);
    }
}

int read_block(void* buffer, block_num num, int offset, int size, FILE* fp){
    get_block(buffer, num, fp);
    if(offset >= BLOCK_SIZE){
        return;
    }
    // Read data only within block boundary
    if(offset+size >= BLOCK_SIZE){
        size =  BLOCK_SIZE - offset;
    }
    char* read_buff[BLOCK_SIZE];
    get_block(read_buff, num, fp);
    memcpy(buffer, &read_buff[offset], size);
    if(ferror(fp))
        return -1;
    return size;
}

void write_block(const void* buffer, block_num num, int offset, int size, FILE* fp){
    if(offset >= BLOCK_SIZE){
        return;
    }
    // Write data only within block boundary
    if(offset+size >= BLOCK_SIZE){
        size =  BLOCK_SIZE - offset;
    }
    char* read_buff[BLOCK_SIZE];
    get_block(read_buff, num, fp);
    memcpy(&read_buff[offset], buffer, size);
    put_block(read_buff, num, fp);
}

/* Write a link block (block that contains list of free blocks) to the disk.
*/
void write_link_block(FILE* fp, block_num num, struct block_list* bl){
    write_block(bl, num, 0, sizeof(struct block_list), fp);
}

// TODO:Need to handle scenario when filesystem is full
/* Update cached list of free blocks when the list becomes empty.
*/
void update_free_blocks_list(struct superblock* sb, block_num block_no, FILE* fp){
    int i;
    struct block_list bl;
    block_num curr_block = block_no;
    int curr_index = FREE_BLOCKS_LIST_SIZE-1;
    int num_entries = BLOCK_SIZE/sizeof(block_num);
    block_num* list = sb->list_free_blocks;
    while(curr_block){
        printf("curr block num=%ld\n", curr_block);
        read_block((void*)&bl, curr_block, 0, sizeof(struct block_list), fp);
        
        for(i = num_entries-1; i >= 0; i--){
            if(curr_index == 0){
                list[0] = curr_block;
                //break;
                write_link_block(fp, curr_block, &bl);
                return;
            }
            else
                if(curr_index == 1  &&  i == 0){
                    list[1] = curr_block;
                    list[0] = bl.list[i];
                    //break;
                    write_link_block(fp, curr_block, &bl);
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
                            //    write_link_block(fp, curr_block, &bl);
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



