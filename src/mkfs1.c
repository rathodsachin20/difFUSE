#include "mkfs1.h"

void mkfs(const char* filepath){
    printf("IN FUNCION mkfs.\n");
    //FILE* fp = fopen(filepath, "w+");
    FILE* fp = fopen(filepath, "rw+");
    if(fp == NULL){
        printf("Error opening file: %s", filepath);
    } 
    fseek(fp, 0L, SEEK_END);
    printf("opened file of size %ld\n", ftell(fp));
    // Write size, num of free blocks etc to superblock
    initialize_superblock(fp);

    // initialize inode structure
    initalize_inodes(fp);

    // initialize list of free blocks
    initialize_free_blocks(superblock.size_fs, fp);

    // write superblock struct to disk after getting all parameters filled
    write_superblock();  //looks like this is not needed, we can write it in initialization itself.

    fclose(fp);
}


/* Initialize superblock struct with initially known fields. Write size, num of free blocks etc.
*/
void initialize_superblock(FILE* fp){
    printf("IN FUNCION initialize_superblock.\n");
    struct superblock * sb = &superblock;
    sb->size_fs = get_fs_size(fp);

    long num_blocks = sb->size_fs / BLOCK_SIZE;
    sb->num_free_blocks = num_blocks - NUM_INODE_BLOCKS - 1;

    // Initialize list of cached free blocks
    int i;
    for(i = 1; i < FREE_BLOCKS_LIST_SIZE; i++)
        sb->list_free_blocks[i] = 0;
    sb->list_free_blocks[0] = 1 + NUM_INODE_BLOCKS;  //Ex. [21,0,0,0,0]

    sb->index_next_free_block = 0;

    // Initialize list of cached free inodes
    for(i = 0; i < FREE_INODES_LIST_SIZE; i++)              // Inode number starts from 1
        sb->list_free_inodes[i] = FREE_INODES_LIST_SIZE - i;    // Ex. [5,4,3,2,1]

    sb->index_next_free_inode = FREE_INODES_LIST_SIZE - 1;

    sb->num_free_inodes = NUM_INODES;

    fseek(fp, 0, SEEK_SET);
    fwrite(&superblock, sizeof(struct superblock), 1, fp);

}

/* Get File system size -------- NEED TO IMPLEMENT
*/
unsigned long get_fs_size(FILE* fp){
    printf("IN FUNCION get_fs_size.\n");
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    printf("FILE SIZE IS:%ld\n", size);
    return (size != -1 ? size : 0);
}

/* Create struct inode for each inode. Initialize it and write to disk.
*/
void initalize_inodes(FILE* fp){
    printf("IN FUNCION initialize_inodes.\n");
    struct inode inode;
    int i, j;

    for(i = 0; i < NUM_INODES; i++){
        //inode->inumber = i;
        inode.owner_id = -1;
        inode.group_id = -1;
        inode.type = (char)0;
        inode.perms = (int)0;
        inode.accessed = 0.0;
        inode.file_modified = 0.0;
        inode.inode_modified = 0.0;
        inode.file_size = 0;

        for(j = 0; j < INODE_NUM_DIRECT_BLOCKS; j++){
            inode.direct_blocks[j] = 0;
        }
        inode.single_indirect_block = 0;
        inode.double_indirect_block = 0;
        inode.triple_indirect_block = 0;
        write_inode(fp, i+1, &inode);
    }
}

void write_inode(FILE* fp, long inumber, struct inode* inodep){
    //printf("IN FUNCION write_inode.\n");
    // Position to seek. Inode number start from 1
    long block_num = ((inumber - 1) / (BLOCK_SIZE / INODE_SIZE)) + 1; //get blk num to write inode in
    short offset = ((inumber - 1) % (BLOCK_SIZE / INODE_SIZE)) * INODE_SIZE; //get offset in that blk
    long pos = (block_num * BLOCK_SIZE) + offset; //get exact pos to begin writing
    fseek(fp, pos, SEEK_SET);
    fwrite(inodep, sizeof(struct inode), 1, fp); //fputs??
    //fflush(fp);   -- IS THIS NEEDED?
}
/* Initialize list of free blocks. Fill link data blocks with entries for free blocks.
*  
*/ 
void initialize_free_blocks(long size_fs, FILE* fp){
    //printf("IN FUNCION initialize_free_blocks.\n");
    long start = 1 + NUM_INODE_BLOCKS;
    //long current  = start;
    long num_blocks = size_fs / BLOCK_SIZE;
    //printf("num_blocks=%ld\n", num_blocks);
    int num_entries = BLOCK_SIZE / sizeof(long);

    struct block_list free_list;
    int i, j;
    for(i=start; i<num_blocks; i+=num_entries){
        if(i <= num_blocks-num_entries){
            for(j=0; j<num_entries; j++){
                free_list.list[j] = i-j+num_entries;
                //printf("Entry: Block[%d][%d]=%ld\t", i, j, free_list.list[j]);
            }
        }
        else{
            for(j=0; j<num_entries; j++){
                long value = i-j+num_entries;
                free_list.list[j] = (value <= num_blocks ? value : 0);
                //printf("Entry: Block[%d][%d]=%ld\t", i, j, free_list.list[j]);
            }

        }
    fseek(fp, BLOCK_SIZE*i, SEEK_SET);
    fwrite(&free_list, sizeof(struct block_list), 1, fp);
    fseek(fp, BLOCK_SIZE*i, SEEK_SET);
    struct block_list mylist;
    int bytes = fread(&mylist, 1, sizeof(struct block_list), fp);
    if(ferror(fp))
        perror("error ");
    fflush(fp);
    //printf("%d bytes read\n", bytes);
    //printf("%ld****************\n", free_list.list[0]);
    //printf("%ld****************\n", mylist.list[0]);

    }

    /*printf("\n............CHECKING READ\n");
    struct block_list llist;
    fseek(fp, BLOCK_SIZE*start, SEEK_SET);
    int bytes = fread(&llist, 1, sizeof(struct block_list), fp);
    //if(bytes==0) perror("error.");
    if(ferror(fp)) perror("error.");
    printf("%d bytes read from block %ld\n", bytes, start);
    printf("%ld--------------\n", *((long*)(&llist)));
    for(i=0;i<num_entries;i++)
        printf(":%ld\t",llist.list[i]);
    printf("\n.............END CHECKING READ\n");
    */


    /* Without using struct 
    long blk_num = start;
    char * char_intp;
    char * block = (char *) malloc( BLOCK_SIZE * sizeof(char));

    for(int i = start; i < num_blocks; i += num_entries){
        int k = i;
        for(int j = 0; j<num_entries; j+= sizeof(long)){
            k -= 1;
            char_intp = (char*)&k;
            memcpy(&block[j], char_intp, sizeof(long));
        }
        fseek(file, i*BLOCK_SIZE, SEEK_SET);
        fwrite(block, BLOCK_SIZE, 1, file); //fputs??

    }
    free(block);
    */

}

/* Write superblock struck to the disk after getting all the parameters filled.
*/
void write_superblock(){
    printf("IN FUNCION write_superblock.\n");

}

