//Implements system calls for the file system

#include "layer2.h"

/* Returns a block number on disk for the given offset in a file, by travesing
*  direct and indirect blocks. Returns 0 when no block exists at given offset.
*/
block_num get_file_block_num(long offset, struct inode node, FILE* fp){
    block_num block_no;
    int n = BLOCK_SIZE / sizeof(block_num); // Number of entries per blocks
    if(offset < BLOCK_SIZE * INODE_NUM_DIRECT_BLOCKS){
        block_no = node.direct_blocks[offset / BLOCK_SIZE];
        if(block_no == 0){
	    printf("invalid offset in get_file_block_num");
            return 0;
        }
    }
    else if(offset < BLOCK_SIZE * (INODE_NUM_DIRECT_BLOCKS + n)){
	indirect_offset = offset - (BLOCK_SIZE * INODE_NUM_DIRECT_BLOCKS);
	block_no = node.single_indirect_block.list[indirect_offset/BLOCK_SIZE];
	if(block_no == 0){
	    printf("invalid offset in get_file_block_num");
	    return 0;
	}
    }
        else{
            // TODO: Double Indirect
            if(offset < BLOCK_SIZE * (INODE_NUM_DIRECT_BLOCKS + n + n*n)){
                block_no = 0;
            }
            else{ // TODO: Triple Indirect
                block_no = 0;
            }
        }
    }
    return block_no;
}


//int add_inode_entry(const char* filepath, block_num file_inum, FILE* fp){
void add_inode_entry(const char* filepath, block_num file_inum, FILE* fp){
    printf("\tin add_inode_entry\n");
    printf("size of struct dir:%ld\n", sizeof(struct directory));
    block_num parent_inum = get_parent_inode_num(filepath, fp);
    struct directory dir;
    struct inode parent_node;
    printf("Here-10\t");
    read_inode(fp, parent_inum, &parent_node);
    //block_num block_num = parent_node.direct_blocks[0];
    block_num block_no = 1+NUM_INODES;
    printf("Here-8\t");
    if(block_no == 0){
        printf("Here-7\t");
        block_no = allocate_block(fp);
        parent_node.direct_blocks[0] = block_no;
        write_inode(fp, parent_inum, &parent_node);
    }
    
    
    printf("Here0\t");
    read_block(&dir, block_no, 0, sizeof(struct directory), fp);
    dir.inode_num[0] = file_inum;
    printf("Here4\t");
    dir.name[0][MAX_FILE_NAME_LEN-1] = '\0';
    strcpy(dir.name[0], &filepath[0]);
    printf("Here5\t");
    write_block(&dir, block_no, 0, sizeof(struct directory), fp);
    struct directory dir2;
    read_block(&dir2, block_no, 0, sizeof(struct directory), fp);
    printf("DIR DISK CONTENTS:%ld, %s\n", dir2.inode_num[0], dir2.name[0]);
    printf("INODE ENTRY ADDED TO PARENT.\n");
    //return 0;
}


block_num get_parent_inode_num(const char* filepath, FILE* fp){
    return 1L;
}


block_num fs_namei(FILE* fp, const char* filep){
    return 2;
    char* fname;
    struct inode working_inode;
    char* filepath;
    int len = strlen(filep);
    block_num inode_num = 0;
    
    filepath = (char*) malloc(sizeof(char)*len);
    strcpy(filepath, filep);
    fname = strtok(filepath,"/");
    
    if(strncmp(&filepath[0],"/",1)==0){
        printf("in fs_namei: path starts with root.");
        while(fname != NULL){
            read_inode(fp, 1, &working_inode);
            //get_block(fp, block_num);
            struct directory dr;
            //loop through the direct blocks to find name
            int n = INODE_NUM_DIRECT_BLOCKS;
            while(n--){
                block_num block_no = working_inode.direct_blocks[n-1];
                if(block_no == 0){
                    printf("NO SUCH FILE!");
                    return 0;
                }
                get_block(&dr, block_no, fp);
                int i;
                for(i=0;i<MAX_NUM_FILE;i++){
                    printf("dir file:%s\n", dr.name[i]);
                    if( strcmp(fname,dr.name[i])==0 ){
                        inode_num = dr.inode_num[i];
                        break;
                    }
                }
            }
            fname = strtok(NULL,"/");
        }
    }
    else{
        printf("in fs_namei: path starts from current dir %s",fname);
        //get curr dir inode ./ 
        //how to access this inode
    }
    printf("In fs_namei: %s: path not found\n", filepath);
    return inode_num;
}

int fs_create(const char *filepath, mode_t mode, struct fuse_file_info * ffi, FILE* fp){
    if(filepath == NULL)
        return -1; 
    //using namei find the directory and the data block to write into
    //namei(fp,filepath);
    
    struct inode node;
    block_num inode_num = allocate_inode(fp, &node);
    printf("in fs_create: got inode no. %ld", inode_num);
    read_inode(fp, inode_num, &node);
    node.owner_id = 121 ;
    node.group_id = 1;
    node.type = 1;
    //node.file_modified = time(0);
    node.direct_blocks[0] = 0;
    //node.direct_blocks[0] = allocate_block(fp);
    write_inode(fp, inode_num, &node);
    
    // Make entry in parent dir
    add_inode_entry(filepath, inode_num, fp);
    printf("File Created.\n");
    return 0;
}

int fs_create_dir(const char *filepath, mode_t mode, struct fuse_file_info * ffi, FILE* fp){
    if(filepath == NULL)
        return -1; //have to check how to return error number
    //using namei find the directory and the data block to write into
    //namei(fp,filepath);
    struct inode nodep;
    block_num inode_num = allocate_inode(fp, &nodep);
    printf("in fs_create_dir: got inode no. %ld\n", inode_num);
    read_inode(fp, inode_num, &nodep);
    nodep.owner_id = 121 ;
    nodep.group_id = 1;
    nodep.type = 2;
    //nodep.file_modified = time(0);
    nodep.direct_blocks[0] = allocate_block(fp);
    write_inode(fp, inode_num, &nodep);
    return 0;
}

int fs_open(const char* filepath, struct fuse_file_info* ffi, FILE* fp){
    //check for errors
    //if file does not exist then create a new file
    //use namei to get inode
    //get the data block using inode
    //write back the data block and inode when done
    block_num inode_num = fs_namei(fp, filepath);
    if(inode_num == 0){
        printf("file doesnt exist, creating...");
        fs_create(filepath, 0, ffi, fp);
    }
    else{
        printf("do nothing");
    }
    
    return 0;
}

int fs_read(const char *filepath, char *buf, size_t count, off_t offset, struct fuse_file_info * ffi, FILE* fp){
    /*check if user has permission to read
     if(inode->file_size < offset)
     return error;
     */
    block_num inode_num = 0;
    block_num block_no = 0;
    inode_num = fs_namei(fp, filepath);
    if(inode_num == 0)
        printf("file does not exist.\n");
    struct inode node;
    read_inode(fp, inode_num, &node);
    block_no = get_file_block_num(offset, node, fp);
    offset = offset % BLOCK_SIZE; // offset within block
    //block_no = nodep.direct_blocks[0];
    read_block(buf, block_no, offset, count, fp);
    return count;
    //get data from block and display
}

int fs_write(const char* filepath, long offset, const char* buffer, long size, FILE* fp){
    // Get inode using silepath
    block_num inode_num = fs_namei(fp, filepath);
    mode_t mode = NULL; // TODO: Change this to defaults
    //struct fuse_file_info  ffi = NULL; // TODO: Change this to incoming parameter
    
    // Write expects file to be already created
    printf("File name:%s inode:%ld\n", filepath, inode_num);
    if(inode_num == 0){
        //fs_create(filepath, mode, ffi, fp);
        //inode_num = fs_namei(fp, filepath);
        printf("%s : No such file exists!\n", filepath);
        return -1;
    }
    
    block_num block_no = 0;
    struct inode node;
    read_inode(fp, inode_num, &node);
    // Calculate from offset the block number and determine if it is direct, indirect block
    block_no = get_file_block_num(offset, node, fp);
    write_block(buffer, block_no, offset % BLOCK_SIZE, size, fp);
    if(ferror(fp))
        perror("Error writing data ");
    fflush(fp);
    
    write_inode(fp, inode_num, &node);
    printf("file write complete.\n");
    return 0;
}

