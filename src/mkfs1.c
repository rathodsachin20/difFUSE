#include <stdio.h>
#include <fuse.h>
#include <fcntl.h>

#define BLOCK_SIZE 512
#define NUM_INODES 20
#define FS_MIN_SIZE 4096
#define INODE_SIZE 64
#define INODE_NUM_DIRECT_BLOCKS 10
#define INODE_NUM_INDIRECT_ADDRS 4
#define NUM_INODE_BLOCKS (NUM_INODES * INODE_SIZE) / BLOCK_SIZE;
#define is_bigendian() ( (*(char*)&i) == 0 )

struct superblock{
    unsigned long size_fs;
    unsigned num_free_blocks;
    unsigned long list_free_blocks[FREE_BLOCKS_LIST_SIZE];
    unsigned index_next_free_block;
    //struct inode **list_free_inodes;
    unsigned long list_free_inodes[FREE_INODES_LIST_SIZE];
    unsigned index_next_free_inode;
    unsigned num_free_inodes; //Total num of free inodes
}superblock;

struct inode{
    //unsigned inode_number; //may not be needed
    int owner_id;
    int group_id;
    char type;
    char perms;
    float accessed;
    float file_modified;
    float inode_modified;
    // ? file_links;
    long int file_size;
    // ? long int size_partition;
    // ? bool file_is_locked;
    long int direct_blocks[INODE_NUM_DIRECT_BLOCKS];
    long int single_indirect_block;
    long int double_indirect_block;
    long int triple_indirect_block;
};


int main(){
    //struct superblock *sb = (struct superblock *) malloc(sizeof(struct superblock));
    struct inode *table_inode = (struct inode *) malloc(sizeof(struct inode)*num_inode);
}

void mkfs(const char* filepath){
    // Write size, num of free blocks etc to superblock
    initialize_superblock();

    // initialize inode structure
    initalize_inodes();

    // initialize list of free blocks
    initialize_free_blocks(superblock.size_fs);

    // write superblock struct to disk after getting all parameters filled
    write_superblock();  //looks like this is not needed, we can write it in initialization itself.
}


/* Initialize superblock struct with initially known fields. Write size, num of free blocks etc.
*/
void initialize_superblock(){
    struct superblock * sb = &superblock;
    sb->size_fs = get_fs_size();

    long num_blocks = sb->size_fs / BLOCK_SIZE;
    sb->num_free_blocks = num_blocks - NUM_INODE_BLOCKS - 1;

    for(int i = 0; i < FREE_BLOCKS_LIST_SIZE-1; i++)
        sb->list_free_blocks[i] = 0;
    sb->list_free_blocks[FREE_BLOCKS_LIST_SIZE-1] = 1 + NUM_INODES_BLOCKS;  //Ex. [0,0,0,0,21]

    sb->index_next_free_block = FREE_BLOCKS_LIST_SIZE - 1;

    for(int i = 0; i < FREE_INODES_LIST_SIZE; i++)              // Inode number starts from 1
        sb->list_free_inodes[i] = FREE_INODES_LIST_SIZE - i;    // Ex. [5,4,3,2,1]

    sb->index_next_free_inode = FREE_INODES_LIST_SIZE - 1;

    sb->num_free_inodes = NUM_INODES;

}

/* Get File system size -------- NEED TO IMPLEMENT
*/
unsigned long get_fs_size(){
    return 0;
}

/* Create struct inode for each inode. Initialize it and write to disk.
*/
void initalize_inodes(){
    struct inode inode;

    for(int i = 0; i < NUM_INODES; i++){
        //inode->inumber = i;
        inode->owner_id = -1;
        inode->group_id = -1;
        inode->type = (char)0;
        inode->perms = (char)0;
        inode->accessed = 0.0;
        inode->file_modified = 0.0;
        inode->inode_modified = 0.0;
        inode->file_size = 0;

        for(int i = 0; i < INODE_NUM_DIRECT_BLOCKS; i++){
            inode->direct_blocks[i] = 0;
        }
        inode->single_indirect_block = 0;
        inode->double_indirect_block = 0;
        inode->triple_indirect_block = 0;
        write_inode(i+1, &inode);
    }
}

void write_inode(FILE* file, long inode_number, struct inode* inodep){
    // Position to seek. Inode number start from 1
    long block_num = ((i - 1) / (BLOCK_SIZE / INODE_SIZE)) + 1;
    short offset = ((i - 1) % (BLOCK_SIZE / INODE_SIZE)) * INODE_SIZE;
    long pos = (block_num * BLOCK_SIZE) + offset;
    fseek(file, pos, SEEK_SET);
    fwrite(inodep, sizeof(struct inode), 1, file); //fputs??
}
/* Initialize list of free blocks. Fill link data blocks with entries for free blocks.
*  
*/ 
void initialize_free_blocks(long size_fs){
    long start = 1 + NUM_INODE_BLOCKS;
    long current  = start;
    long num_blocks = size_fs / BLOCK_SIZE;
    int num_entries = BLOCK_SIZE / sizeof(long);

    long blk_num = start;
    char * char_intp;
    char * block = (char *) malloc( BLOCK_SIZE * sizeof(char));

    for(int i = start; i < num_blocks; i += num_entries){
        blk_num++;
        char_intp = (char*)&blknum;
        for(int j = BLOCK_SIZE-1-sizeof(long); j>=0; j += sizeof(long)){
            memcpy(&block[j], char_intp, sizeof(long));
        }
        fseek(file, i*BLOCK_SIZE, SEEK_SET);
        fwrite(block, BLOCK_SIZE, 1, file); //fputs??

    }
    free(block);

}

/* Write superblock struck to the disk after getting all the parameters filled.
*/
void write_superblock(){

}

