#include <stdio.h>
#include "global.h"

/* Layer 0 is the interface to the disk. It implements block abstraction.
*  All requests to disk should go through layer 0.
*/

void get_block(void* buffer, block_num num, FILE* fp){
    fseek(fp, num * BLOCK_SIZE, SEEK_SET);
    fread(buffer, BLOCK_SIZE, 1, fp); //Buffer should have memory allocated already
    if(ferror(fp))
        perror("error ");
    fflush(fp);   //-- IS THIS NEEDED?
}

void put_block(const void* buffer, block_num num, int offset, int size, FILE* fp){
    fseek(fp, num * BLOCK_SIZE + offset, SEEK_SET);
    fwrite(buffer, size, 1, fp); //Buffer should have memory allocated already
    if(ferror(fp))
        perror("error ");
    fflush(fp);
}
