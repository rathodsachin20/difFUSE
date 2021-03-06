#include <stdio.h>
#include "layer0.h"

/* Layer 0 is the interface to the disk. It implements block abstraction.
*  All requests to disk should go through layer 0.
*/
/*
int get_block(void* buffer, block_num num, FILE* fp){
    fseek(fp, num * BLOCK_SIZE, SEEK_SET);
    fread(buffer, BLOCK_SIZE, 1, fp); //Buffer should have memory allocated already
    if(ferror(fp)){
        perror("error ");
        return -1;
    }
    fflush(fp);
    return BLOCK_SIZE;
}

void put_block(const void* buffer, block_num num, FILE* fp){
    fseek(fp, num * BLOCK_SIZE, SEEK_SET);
    fwrite(buffer, BLOCK_SIZE, 1, fp); //Buffer should have memory allocated already
    if(ferror(fp))
        perror("error ");
    fflush(fp);
}
*/
int get_block(void* buffer, block_num num){
    lseek(fs_desc, num * BLOCK_SIZE, SEEK_SET);
    size_t size = read(fs_desc, buffer, BLOCK_SIZE); //Buffer should have memory allocated already
    if(size < 0){
        perror("READ ERROR!\n");
        return size;
    }
    return BLOCK_SIZE;
}

size_t put_block(const void* buffer, block_num num){
    lseek(fs_desc, num * BLOCK_SIZE, SEEK_SET);
    size_t size = write(fs_desc, buffer, BLOCK_SIZE); //Buffer should have memory allocated already
    if(size < 0){
        perror("WRITE ERROR!\n");
    }
    return size;
}
