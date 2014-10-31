#include <stdio.h>
#include <sys/stat.h>
#include <fuse.h>

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
