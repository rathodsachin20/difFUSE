#ifndef __fs_utility_c__
#define __fs_utility_c__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "global.h"

long int fs_namei(FILE* fp, const char* filep){
    char* fname;
    struct inode working_inode;  
    char * filepath;
    int len = strlen(filep);
    long int inode_num = 0;
    
    filepath = (char*) malloc(sizeof(char)*len);
    strcpy(filepath, filep);
    fname = strtok(filepath,"/");
    
    if(strncmp(&filepath[0],"/",1)==0){
        printf("in fs_namei: path starts with root.");
        //start from root
        get_inode_struct(fp, 1, &working_inode);
        //long int block_num = working_inode->direct_blocks[0];
        //get_block(fp, block_num);
        struct directory dr;
        long block_num = working_inode.direct_blocks[0];
        if(block_num == 0){
            printf("NO SUCH FILE!");
            return 0;
        }
        fseek(fp, BLOCK_SIZE*block_num, SEEK_SET);
        fread(&dr, sizeof(struct directory), 1, fp);
        int i=0;
        while(i<MAX_NUM_FILE){
            printf("dir file:%s\n", dr.name[i]);
            if( strcmp(fname,dr.name[i])==0 ){
                inode_num = dr.inode_num[i];
                break;
            }
            i++;
        }
    }
    printf("In fs_namei: %s: path not found\n", filepath);
    return inode_num;
}
    /*
    fname = strtok(filepath,"/");
    while(fname != NULL){
        printf("fetching file.. %s\n",fname);
        //search for file starting from root
    //
    //if file not found then error.
        fname = strtok (NULL, "/");
    }
    num_inode = get_inode_n
    return num_inode;
}

struct inode* get_inode(){

//implement get_block
//implement put_inode
*/

#endif
