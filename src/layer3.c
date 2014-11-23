#include "layer2.h"

//static const char *hello_str = "Hello World!\n";
//static const char *hello_path = "/hello";
FILE* fp;

static int hello_getattr(const char *path, struct stat *stbuf)
{
	return fs_getattr(path, stbuf, fp);
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	return fs_readdir(path, buf, filler, offset, fi, fp);
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	if (fs_namei(fp, path) == 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	(void) fi;
    int read_size = fs_read(path, buf, size, offset, fp);
	return read_size;
}

static struct fuse_operations hello_oper = {
	.getattr	= hello_getattr,
	.readdir	= hello_readdir,
	.open		= hello_open,
	.read		= hello_read,
};

int main(int argc, char *argv[])
{
    char filesystem[1024];
    strcpy(filesystem, argv[1]);
    fp = fopen(filesystem, "rw+");
	return fuse_main(argc, argv, &hello_oper, NULL);
}
