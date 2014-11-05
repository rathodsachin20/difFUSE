#ifndef __fs_c__
#define __fs_c__

#include "fs.h"
//#include "fsutil.c"


int fs_write(const char* filepath, long offset, const char* buffer, long size, FILE* fp){
    // Get inode using silepath
    long inode_num = fs_namei(fp, filepath);
    mode_t mode = NULL; // TODO: Change this to defaults
    //struct fuse_file_info  ffi = NULL; // TODO: Change this to incoming parameter

    // Write expects file to be already created
    if(inode_num == 0){
        //fs_create(filepath, mode, ffi, fp);
        //inode_num = fs_namei(fp, filepath);
        printf("%s : No such file exists!\n", filepath);
        return -1;
    }

    long block_num = 0;
    int block_pos = 0;
    struct inode node;
    get_inode_struct(fp, inode_num, &node);
    // Calculate from offset the block number and determine if it is direct, indirect block
    block_num = get_file_block_num(offset, node, &block_pos, fp);
    long block_offset = (block_num * BLOCK_SIZE) + (offset % BLOCK_SIZE);
    fseek(fp, block_offset, SEEK_SET);
    fwrite(buffer, size, 1, fp);
    if(ferror(fp))
        perror("Error writing data ");
    fflush(fp);

    write_inode(fp, inode_num, &node);

    return 0;
}


long get_file_block_num(long offset, struct inode node, int* block_pos, FILE* fp){
    long block_num;
    int n = BLOCK_SIZE / sizeof(long); // Number of entries per blocks
    if(offset < BLOCK_SIZE * INODE_NUM_DIRECT_BLOCKS){
        block_num = node.direct_blocks[offset / BLOCK_SIZE];
        *block_pos = block_num;
        if(block_num == 0){
           block_num = get_free_block_num(fp);
           // Update Inode entry for block
           node.direct_blocks[*block_pos] = block_num;
        }
    }
    else // TODO:Single Indirect
    if(offset < BLOCK_SIZE * (INODE_NUM_DIRECT_BLOCKS + n)){
        block_num = 0;
    }
    else
    if(offset < BLOCK_SIZE * (INODE_NUM_DIRECT_BLOCKS + n + n*n)){ // TODO: Double indirect
        block_num = 0;
    }
    else{ // TODO: Triple Indirect
        block_num = 0;
    }
}

#endif
