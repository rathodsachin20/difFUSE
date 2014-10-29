#include <stdio.h>
#include <fuse.h>
#include <time.h>
#include <fcntl.h>

#define NUM_INODE 20
#define FS_MIN_SIZE 4096
#define INODE_SIZE 64
#define BLOCK_SIZE 512
#define INODE_NUM_DIRECT_ADDRS 8
#define INODE_NUM_INDIRECT_ADDRS 4

typedef struct superblock{
unsigned size_fs;
unsigned num_free_blocks;
unsigned list_free_blocks;
struct inode **list_free_inodes;
unsigned size_inode_list;
unsigned index_next_free_block;
unsigned index_next_free_inode;

}superblock;

typedef struct inode{
unsigned inode_number;
unsigned inode_size;
int file_owner;
int file_type;
file_permissions;
time_t file_access_time;
file_links;
unsigned long file_size;
long int size_partition;
bool file_is_locked;

}inode;


int main(){
struct superblock *sb = (struct superblock *) malloc(sizeof(struct superblock));
struct inode *table_inode = (struct inode *) malloc(sizeof(struct inode)*num_inode);
}

struct inode* inode_allocate(int file_owner, int file_type, ){
//remove inode from free list
//initialize inode
//return
}

struct inode* inode_get(int file_owner, ){
//if user has permissions then pass inode requested
//convert path name to file name using namei
//return
void mkfs(const char* filepath){
    fopen()
    //Write size, num of free blocks etc

    //initialize list of free blocks

    //initialize inode structure

    //initialize list of free inodes
}

struct inode* get_inode(int file_owner, ){

}

