#include <stdio.h>
#include <fuse.h>

int fs_getattr(const char *filename, struct stat *st){
    struct node* inode = get_inode(filename);
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
