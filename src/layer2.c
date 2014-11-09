//Implements system calls for the file system

#include "global.h"


int fs_open(const char* filepath, struct fuse_file_info* ffi, FILE* fp){
    //check for errors
    //if file does not exist then create a new file
    //use namei to get inode
    //get the data block using inode
    //write back the data block and inode when done
    long int inode_num = fs_namei(fp, filepath);
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
    long int inode_num = 0, block_num=0;
    inode_num = fs_namei(fp, filepath);
    if(inode_num == 0)
        printf("file does not exist");
    struct inode nodep;
    get_inode_struct(fp, inode_num, &nodep);
    block_num = nodep.direct_blocks[0];
    fseek(fp, BLOCK_SIZE * block_num + offset, SEEK_SET);
    fread(buf, count, 1, fp);
    //get data from block and display
}

int fs_create(const char *filepath, mode_t mode, struct fuse_file_info * ffi, FILE* fp){
    if(filepath == NULL)
        return -1; //have to check how to return error number
    //using namei find the directory and the data block to write into
    //namei(fp,filepath);
    
    struct inode node;
    long int inode_num = get_free_inode(fp, &node);
    printf("in fs_create: got inode no. %ld", inode_num);
    get_inode_struct(fp, inode_num, &node);
    node.owner_id = 121 ;
    node.group_id = 1;
    node.type = 1;
    //node.file_modified = time(0);
    node.direct_blocks[0] = 0;
    //node.direct_blocks[0] = get_free_block_num(fp);
    write_inode(fp, inode_num, &node);
    
    // Make entry in parent dir
    add_inode_entry(filepath, inode_num, fp);
    return 0;
}

//int add_inode_entry(const char* filepath, long file_inum, FILE* fp){
void add_inode_entry(const char* filepath, long file_inum, FILE* fp){
    printf("\tin add_inode_entry\n");
    printf("size of struct dir:%ld\n", sizeof(struct directory));
    long parent_inum = get_parent_inode_num(filepath, fp);
    struct directory dir;
    struct inode parent_node;
    printf("Here-10\t");
    get_inode_struct(fp, parent_inum, &parent_node);
    //long block_num = parent_node.direct_blocks[0];
    long block_num = 1+NUM_INODES;
    printf("Here-8\t");
    if(block_num == 0){
        printf("Here-7\t");
        block_num = get_free_block_num(fp);
        parent_node.direct_blocks[0] = block_num;
        write_inode(fp, parent_inum, &parent_node);
    }
    
    
    printf("Here0\t");
    fseek(fp, BLOCK_SIZE*block_num, SEEK_SET);
    fread(&dir, BLOCK_SIZE , 1, fp);
    if(ferror(fp))perror(" errr ");
    dir.inode_num[0] = file_inum;
    printf("Here4\t");
    dir.name[0][MAX_FILE_NAME_LEN-1] = '\0';
    strcpy(dir.name[0], &filepath[0]);
    printf("Here5\t");
    fseek(fp, BLOCK_SIZE*block_num, SEEK_SET);
    fwrite(&dir, BLOCK_SIZE, 1, fp);
    if(ferror(fp)){
        perror(" errr ");
    }
    struct directory dir2;
    fseek(fp, BLOCK_SIZE*block_num, SEEK_SET);
    fread(&dir2, BLOCK_SIZE , 1, fp);
    if(ferror(fp))perror(" errr ");
    printf("DIR DISK CONTENTS:%ld, %s\n", dir2.inode_num[0], dir2.name[0]);
    printf("INODE ENTRY ADDED TO PARENT.\n");
    //return 0;
}


int fs_create_dir(const char *filepath, mode_t mode, struct fuse_file_info * ffi, FILE* fp){
    if(filepath == NULL)
        return -1; //have to check how to return error number
    //using namei find the directory and the data block to write into
    //namei(fp,filepath);
    struct inode nodep;
    long int inode_num = get_free_inode(fp, &nodep);
    printf("in fs_create_dir: got inode no. %ld\n", inode_num);
    get_inode_struct(fp, inode_num, &nodep);
    nodep.owner_id = 121 ;
    nodep.group_id = 1;
    nodep.type = 2;
    //nodep.file_modified = time(0);
    nodep.direct_blocks[0] = get_free_block_num(fp);
    write_inode(fp, inode_num, &nodep);
    return 0;
}

long get_parent_inode_num(const char* filepath, FILE* fp){
    return 1L;
}
