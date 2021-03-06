#include "layer1.h"

/* Get a new inode struct
*/
block_num allocate_inode(struct inode* nodep){
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
        sb->list_free_inodes[index] = 0;
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
            sb->list_free_inodes[index] = 0;
            int16_t curr_index = FREE_INODES_LIST_SIZE-1;
            sb->index_next_free_inode = curr_index;
            // Get new inodes
            while(curr_inode_num < NUM_INODES){
                //printf("Curr_inode_num=%ld\n", curr_inode_num);
                if(curr_index < 0)
                    break;
                read_inode(curr_inode_num, &curr_inode);
                if(curr_inode.mode == 0){
                    //printf("Free inode_num=%ld\n", curr_inode_num);
                    sb->list_free_inodes[curr_index] = curr_inode_num;
                    curr_index--;
                }
                curr_inode_num++;
                if(curr_inode_num == NUM_INODES){
                    //printf("Last inode added. curr_index=%d\n",curr_index);
                    for(; curr_index>=0; curr_index--){
                        sb->list_free_inodes[curr_index] = 0;
                    }
                }
            }
        }
    }
    read_inode(inode_num, nodep);
    //nodep->type = 1;
    //nodep->mode = S_IFREG;
    write_inode(inode_num, nodep);
    printf("Giving inode no. %ld\t", inode_num);
    sb->num_free_inodes -= 1;
    return inode_num;
}

/* Free an inode in use. Add it to free inodes list.
*/
void free_inode(block_num inumber){
    if(inumber > 1 + NUM_INODES || inumber <1){
        return;
    }
    // Make type=0 and write to disk - NO! it's duplicate
    printf("<----------- FREEING INODE NO %ld ------------->\n", inumber);
    //struct inode node;
    //node.type = 0;
    //write_inode(inumber, &node);
    
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
            struct inode node = {};
            node.mode = 0;
            write_inode(inumber, &node);
        }
    }
    
}

/* Read contents of inode into inode struct
*/
void read_inode(block_num inode_num, struct inode* nodep){
    if(inode_num < 1 || inode_num > NUM_INODES){
        printf("Invalid Inode Number.\n");
        return;
    }
    block_num block_no = ((inode_num - 1) / (BLOCK_SIZE/INODE_SIZE)) + 1;
    int offset = ((inode_num - 1) % (BLOCK_SIZE/INODE_SIZE)) * INODE_SIZE;
    read_block((void*) nodep, block_no, offset, sizeof(struct inode));
}

block_num allocate_block(){
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
        //get_new_free_blocks_list(&newlist, block_no );
        update_free_blocks_list(sb, block_no);
        block_no = sb->list_free_blocks[FREE_BLOCKS_LIST_SIZE-1];
        sb->index_next_free_block = FREE_BLOCKS_LIST_SIZE-2;
        //int i;
        //for(i=0; i<FREE_BLOCKS_LIST_SIZE; i++){
        //    sb->list_free_blocks[i] = newlist[i];
        //}
    }
    printf("Gave block num %ld\n", block_no);
    return block_no;
}

/* Free a block in use. Add it to free blocks list.
*/
void free_block(block_num block_no){
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
        //int i;
        struct block_list bl = {};
        bl.list[0] = sb->list_free_blocks[0];
        sb->list_free_blocks[0] = block_no;
        //for(i=1; i<BLOCK_SIZE/sizeof(block_num); i++){
        //    bl.list[i] = 0L;
        //}
        write_link_block(block_no, &bl);
    }
}

int read_block(void* buffer, block_num num, int offset, int size){
    //get_block(buffer, num );
    if(offset >= BLOCK_SIZE){
        return -1;
    }
    // Read data only within block boundary
    if(offset+size > BLOCK_SIZE){
        size =  BLOCK_SIZE - offset;
    }
    char read_buff[BLOCK_SIZE];
    get_block(read_buff, num);
    memcpy(buffer, &read_buff[offset], size);

    return size;
}

/* Write a link block (block that contains list of free blocks) to the disk.
*/
void write_link_block(block_num num, struct block_list* bl){
    write_block(bl, num, 0, sizeof(struct block_list));
}

// TODO:Need to handle scenario when filesystem is full
/* Update cached list of free blocks when the list becomes empty.
*/
void update_free_blocks_list(struct superblock* sb, block_num block_no){
    int i;
    struct block_list bl = {};
    block_num curr_block = block_no;
    int curr_index = FREE_BLOCKS_LIST_SIZE-1;
    int num_entries = BLOCK_SIZE/sizeof(block_num);
    block_num* list = sb->list_free_blocks;
    block_num prev_block = 0;
    while(curr_block){
        if(prev_block==curr_block){
            break;
        }
        prev_block = curr_block;
        printf("curr block num=%ld\n", curr_block);
        read_block((void*)&bl, curr_block, 0, sizeof(struct block_list));
    printf("Printing this block:\n");
    for(i=0;i<num_entries;i++){
        printf("%ld--", bl.list[i]);
    }
        
        for(i = num_entries-1; i >= 0; i--){
            if(curr_index == 0){
                printf("allocate_block:curr_index is 0.next link block:%ld\n",curr_block);
                printf("list[1]:%ld\n",list[1]);
                list[0] = curr_block;
                //break;
                write_link_block(curr_block, &bl);
                return;
            }
            else
                /*if(curr_index == 1  &&  i == 0){
                    list[1] = curr_block;
                    list[0] = bl.list[i];
                    //break;
                    write_link_block(curr_block, &bl);
                    return;
                }*/
                if(curr_index == 1){
     printf("allocate_block:curr_index is 1:curr_block%ld\n",curr_block);
                    while(bl.list[i]==0 && i>0){
                        i--;
                    }
                    if(i==0){
                        list[1] = curr_block;
                        list[0] = bl.list[i];
                        //printf("--list[1]:%ld\n",list[1]);
                        //break;
                        write_link_block(curr_block, &bl);
                        return;
                    }
                    else{
                        list[1] = bl.list[i];
                        bl.list[i]=0;
                        curr_index--;
                        continue;
                    }
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
                            //    write_link_block(curr_block, &bl);
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

block_num allocate_block_list(){
    block_num block_no = allocate_block();
    if(!block_no) return 0;
    //struct directory dir;
    struct block_list bl = {};
    //int i;
    //for(i=0; i<(BLOCK_SIZE/sizeof(block_num)); i++){
        //printf("**direntry[%d]:%ld**",i,bl.list[i]);
        //bl.list[i] = 0;
    //}
    write_block(&bl, block_no, 0, sizeof(struct block_list));
    return block_no;
}

int read_block_list(void* bl, block_num bn){
    return read_block(bl, bn, 0, sizeof(struct block_list));
}

void write_block_list(const void* bl, block_num bn){
    write_block(bl, bn, 0, sizeof(struct block_list));
}

void initialize_dir_block(block_num block_no){
    struct directory dir = {};
    int i;
    for(i=0; i<BLOCK_SIZE/NAMEI_ENTRY_SIZE; i++){
        dir.inode_num[i] = 0;
        strcpy(dir.name[i], "");
    }
    write_block(&dir, block_no, 0, sizeof(struct directory));
}
