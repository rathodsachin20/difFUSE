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
    int write_size = fs_write(path, offset, buf, size);
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

static struct fuse_operations dif_oper = {
    .getattr    = dif_getattr,
    .readdir    = dif_readdir,
//    .open       = dif_open,
//    .release    = dif_release,
    .read       = dif_read,
    .create     = dif_create,
    .write      = dif_write,
    .mkdir = dif_mkdir,
};

int main(int argc, char *argv[])
{
    int size = 1024*1024*100;
    FILE* fp = fopen("test1.txt", "w+");
    if(ferror(fp)){perror("error ");}
    fseek(fp, size, SEEK_SET);
    fputc('\n', fp);
    fseek(fp, 0L, SEEK_END);
    printf("created file of size %ld\n", ftell(fp));
    fclose(fp);

    fs_desc = open("test1.txt", O_RDWR|O_CREAT );

    mkfs("test1.txt");

    int LARGE_SIZE = 512*40;
    char text[] = "Hello world";
    char largetext[LARGE_SIZE];
    int i;
    mode_t mode_d = S_IFDIR | 0777;
    mode_t mode = S_IFREG | 0666;
    for(i=0; i<LARGE_SIZE; i++){
        largetext[i] = (char)(i%26+63);
    }
    largetext[i] = '\0';
    //printf("LARGETEXT:%s\n", largetext);

    printf("11th:%c...\n", text[11]);
    fs_create_dir("/", mode_d); // Move creation of root dir to mkfs

    fs_create("/tt.txt", mode );
    fs_write("/tt.txt", 0, text, 11);
    //char read_buff[15];
    //fs_read("/tt.txt", read_buff, 11, 0);
    //printf(" FILE 1 CONTENTS:%s\n\n\n", read_buff);

    char text1[] = "I am back!";
    fs_create("/ss.txt", mode);
    fs_write("/ss.txt", 0, text1, 11);
    //char read_buff1[15];
    //fs_read("/ss.txt", read_buff1, 11, 0);
    //printf(" FILE 2 CONTENTS:%s\n\n\n", read_buff1);

    //strcpy(read_buff, "Garbage!!");
    //fs_read("/tt.txt", read_buff, 11, 0);
    //printf(" FILE 1 CONTENTS:%s\n\n\n", read_buff);

    fs_create("/large.txt", mode);
    fs_write("/large.txt", 0, largetext, LARGE_SIZE);
    char read_buff_large[LARGE_SIZE];
    fs_read("/large.txt", read_buff_large, LARGE_SIZE, 0);
    read_buff_large[LARGE_SIZE-1] = '\0';
    //printf(" FILE 4 CONTENTS:%s\n\n\n", read_buff_large);

//char buf[10240];
//fuse_fill_dir_t filler;
//fs_readdir("/", buf, filler, 0, NULL);

    //mkfs("t");
    //char filesystem[1024];
    //strcpy(filesystem, "");//argv[1]);
    //fp = fopen("/home/sachin/euca/diffuse/src/t", "rw+");
    //fp = (FILE*)opendir("/home/sachin/euca/diffuse/src/t");
    //fp = (FILE*)opendir("/home/sachin/euca/diffuse/src/t");
    //printf("fp:%p\n",fp);
    return fuse_main(argc, argv, &dif_oper, NULL);
}
