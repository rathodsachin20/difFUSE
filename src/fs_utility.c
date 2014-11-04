#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "global.h"

long int fs_namei(FILE* fp, const char* filep){
    char* fname;
    struct node* working_inode;  
    char * filepath = filep;
    fname = strtok(filepath,"/");
    if(strcmp(filepath[0],"/")==0){
	//start from root
	get_inode_struct(fp, 1, &working_inode);
	//long int block_num = working_inode->direct_blocks[0];
	//get_block(fp, block_num);
	struct directory dr;
	long int inode_num=0;
	fread(&dr, sizeof(struct directory), 1, fp);
	int i=0;
	while(i<MAX_NUM_FILE){
	    if( strcmp(fname,dr.name[i])==0 ){
		inode_num = dr.inode_num[i];
		break;
	    }
	    i++;
	}
	return inode_num;
    }
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
