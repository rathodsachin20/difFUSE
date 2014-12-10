#include "mkfs1.h"
//#include "layer0.c"

void mkfs(const char* filepath){
    printf("IN FUNCION mkfs.\n");
    int fd = open(filepath, O_RDWR);
    if(fd < 0){
        printf("Error opening file: %s\t", filepath);
        return;
    } 
    lseek(fd, 0L, SEEK_END);
    // Write size, num of free blocks etc to superblock
    initialize_superblock(fd);

    // initialize inode structure
    initalize_inodes();

    // initialize list of free blocks
    initialize_free_blocks(superblock.size_fs);

    // write superblock struct to disk after getting all parameters filled
    //write_superblock();  //looks like this is not needed, we can write it in initialization itself.

    close(fd);
    printf("MKFS finished.\n");
    return;
}


/* Initialize superblock struct with initially known fields. Write size, num of free blocks etc.
*/
void initialize_superblock(int fd){
    printf("IN FUNCION initialize_superblock.\n");
    struct superblock * sb = &superblock;
    sb->size_fs = get_fs_size(fd);

    long num_blocks = sb->size_fs / BLOCK_SIZE;
    sb->num_free_blocks = num_blocks - NUM_INODE_BLOCKS - 1;

    printf("No. of free blocks---->%ld\n",sb->num_free_blocks);
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
printf("writing\n");
    write_block(&superblock, 0, 0, sizeof(struct superblock));

}

/* Get File system size -------- NEED TO IMPLEMENT
*/
unsigned long get_fs_size(int fd){
    struct stat fileStat;
    if(fstat(fd, &fileStat) < 0){
        return -1;
    }
    printf("FILE SIZE IS:%ld\n", fileStat.st_size);
    return fileStat.st_size;
}
/*unsigned long get_fs_size(FILE* fp){
    printf("IN FUNCION get_fs_size.\n");
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    printf("FILE SIZE IS:%ld\n", size);
    return (size != -1 ? size : 0);
}*/

/* Create struct inode for each inode. Initialize it and write to disk.
*/
void initalize_inodes(){
    printf("IN FUNCION initialize_inodes.\n");
    struct inode inode;
    int i, j;

    for(i = 0; i < NUM_INODES; i++){
        //inode->inumber = i;
        inode.owner_id = 0;
        inode.group_id = 0;
        //inode.type = (char)0;
        //inode.perms = (int16_t)0;
        inode.mode = 0;
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
        inode.last_filled_block_index = 0;
        write_inode(i+1, &inode);
    }
}

void write_inode(block_num inumber, struct inode* inodep){
    //printf("IN FUNCION write_inode.\n");
    // Position to seek. Inode number start from 1
    long block_no = ((inumber - 1) / (BLOCK_SIZE / INODE_SIZE)) + 1; //get blk num to write inode in
    short offset = ((inumber - 1) % (BLOCK_SIZE / INODE_SIZE)) * INODE_SIZE; //get offset in that blk
    write_block(inodep, block_no, offset, sizeof(struct inode));

}

size_t write_block(const void* buffer, block_num num, int offset, int size){
    if(offset >= BLOCK_SIZE){
        return 0;
    }
    // Write data only within block boundary
    if(offset+size >= BLOCK_SIZE){
        size =  BLOCK_SIZE - offset;
    }
    char read_buff[BLOCK_SIZE];
    get_block(read_buff, num);
    memcpy(&read_buff[offset], buffer, size);
    //printf("Buffer:%s. Readbuff:%s\n", (char*)buffer, read_buff);
    return put_block(read_buff, num);
}

/* Initialize list of free blocks. Fill link data blocks with entries for free blocks.
*  
*/ 
void initialize_free_blocks(long size_fs){
    printf("IN FUNCION initialize_free_blocks.\n");
    long start = 1 + NUM_INODE_BLOCKS;
    //long current  = start;
    long num_blocks = size_fs / BLOCK_SIZE;
    //printf("num_blocks=%ld\n", num_blocks);
    int num_entries = BLOCK_SIZE / sizeof(block_num);

    struct block_list free_list={};
    int i, j;
    for(i=start; i<num_blocks; i+=num_entries){
        if(i <= num_blocks-num_entries){
            for(j=0; j<num_entries; j++){
                free_list.list[j] = i-j+num_entries;
                if(i<start+2*num_entries)printf("Entry: Block[%d][%d]=%ld\t", i, j, free_list.list[j]);
            }
        }
        else{
            for(j=0; j<num_entries; j++){
                long value = i-j+num_entries;
                free_list.list[j] = (value <= num_blocks ? value : 0);
                //printf("Entry: Block[%d][%d]=%ld\t", i, j, free_list.list[j]);
            }

        }
    write_block(&free_list, i, 0, sizeof(struct block_list));
    //fseek(fp, BLOCK_SIZE*i, SEEK_SET);
    //struct block_list mylist;
    //int bytes = fread(&mylist, 1, sizeof(struct block_list), fp);
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
    printf("Finished initialize free blocks\n");
}


