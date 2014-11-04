#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "global.h"
#include <fuse.h>
#include <fuse_common.h>
#include <time.h>

int fs_getattr(const char *filepath, struct stat *st){
    struct node* inode = get_inode(filepath);
    //if fails then return error
    st->st_dev = inode->;//Id of device containing file
    st->st_ino = inode->inumber;
    st->st_mode = inode->perms;//protection
    st->st_nlink = inode->;//number of hard links
    st->st_rdev = inode->
    st->st_uid = inode->owner_id;
    st->st_gid = inode->group_id; 
    st->st_size = inode->file_size;
    st->st_atime = inode->accessed;
    st->st_mtime = inode->file_modified;

    return 0;
}

int fs_create(const char *filepath, mode_t mode, struct fuse_file_info *, FILE* fp){
    if(filepath == NULL)
	return ENOENT; //have to check how to return error number
    //using namei find the directory and the data block to write into
    //namei(fp,filepath);
    struct inode nodep;
    int inode_num = get_free_inode(fp, &nodep);
    get_inode_struct(fp, inode_num, &nodep);
    nodep.ownder_id = 1 ;
    nodep.group_id = 1;
    nodep.type = 1;
    nodep.file_modified = time(0);
    nodep.direct_blocks[0] = get_free_block_num(fp);
    write_inode(fp, inode_num, &nodep);
    return 0;
}

//don't know if mknod needs to be implemented or create is sufficient!
int fs_mknod(const char *filepath, mode_t mode, dev_t dev){
    if(path is empty string)
	return ENOENT
    if(file name already exists)
	return EEXIST
    if(component of path denied search/write permission)
	return EACCES

    //fetch and assign inode to this new file
    //set permissions
    //assign group id of file to that of parent group id

    return 0;
}

int fs_open(const char* filepath, struct fuse_file_info* ffi, FILE* fp){
    //check for errors
    //if file does not exist then create a new file
    //get inode
    //get the data block using inode
    

    return fd; //return file descriptor
}

int fs_read(const char *filepath, char *buf, size_t count, off_t offset, struct fuse_file_info * ffi, FILE* fp){
//check if user has permission to read
struct inode* inode = get_inode(filepath)
if(inode->file_size < offset)
    return error;
//get data from block and display
}
