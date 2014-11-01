#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fuse.h>

uint32_t fs_namei(const char* filepath){
    char *fname;
    fname = strtok(filepath,"/");
    while(fname != NULL){
        printf("fetching file.. %s\n",fname);
        
        fname = strtok (NULL, "/");
    }
    
    return num_inode;
}
//implement get_inode

struct inode* get_inode();

//implement get_block
//implement put_inode