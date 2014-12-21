#include "layer2.h"
#include <dirent.h>

//static const char *hello_str = "Hello World!\n";
//static const char *hello_path = "/hello";
//FILE* fp;

static int dif_getattr(const char *path, struct stat *stbuf)
{
    return fs_getattr(path, stbuf);
}

static int dif_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi)
{
    return fs_readdir(path, buf, filler, offset, fi);
}

static int dif_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{
    (void) fi;
    int read_size = fs_read(path, buf, size, offset);
    return read_size;
}

static int dif_write(const char *path, const char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{
    (void) fi;
    mode_t mode = 0;
    //if (fs_namei(path) == 0){
    //    fs_create(path, mode);
    //}
    printf("Writing %d bytes\n", (int)size);
    int write_size = fs_write(path, offset, buf, size);
    printf("Wrote %d bytes\n", write_size);
    return write_size;
}

static int dif_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    (void) fi;
    if (fs_namei(path) != 0)
        return -ENOENT;
    return fs_create(path, mode);
}

static int dif_open(const char *path, struct fuse_file_info *fi)
{
    if (fs_namei(path) == 0)
        return -ENOENT;

    if ((fi->flags & 3) != O_RDONLY)
        return -EACCES;

    return 0;
}

static int dif_release(const char *path, struct fuse_file_info *fi)
{
    if (fs_namei(path) == 0)
        return -ENOENT;

    return 0;
}

static int dif_mkdir(const char *path, mode_t mode)
{
    if(fs_namei(path) != 0)
        return -ENOENT;
    return fs_create_dir(path, mode);
}

static int dif_unlink(const char *path)
{
    if(fs_namei(path) == 0)
	return -ENOENT;
    return fs_unlink(path);
}

static int dif_rmdir(const char* path){
    if(fs_namei(path) == 0)
	return -ENOENT;
    return fs_rmdir(path);
}

int dif_utimens(const char * path, const struct timespec tv[2]){
    if(fs_namei(path) == 0)
        return -ENOENT;
    return fs_mod_time(path, tv);
}

int dif_rename(const char* oldpath, const char* newpath){
    if(fs_namei(oldpath) == 0)
	return -ENOENT;
    return fs_rename(oldpath, newpath);
}

int dif_chmod(const char* path, mode_t mode){
    if(fs_namei(path) == 0)
	return -ENOENT;
    return fs_chmod(path, mode);
}

static struct fuse_operations dif_oper = {
    .getattr    = dif_getattr,
    .readdir    = dif_readdir,
//    .open       = dif_open,
//    .release    = dif_release,
    .read       = dif_read,
    .create     = dif_create,
    .write      = dif_write,
    .mkdir = dif_mkdir,
    .unlink = dif_unlink,
    .rmdir = dif_rmdir,
    .utimens = dif_utimens,
    .rename = dif_rename,
    .chmod = dif_chmod,
};

int main(int argc, char *argv[])
{
    char device[] = "/dev/vdb";
    //FILE* fp = fopen(device, "w+");
    //printf("VDB size %ld\n", ftell(fp));
    //fclose(fp);

    fs_desc = open(device, O_RDWR|O_CREAT );
    if(fs_desc<0){
        printf("Error opening file:%s\n",device);
    }
    //mkfs(device);
    mkfs(device);
    //fs_desc = open(device, O_RDWR|O_CREAT );

    mode_t mode_d = S_IFDIR | 0777;
    fs_create_dir("/", mode_d); // Move creation of root dir to mkfs
    
    return fuse_main(argc, argv, &dif_oper, NULL);
}
