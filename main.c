#define FUSE_USE_VERSION	26

#include <stdio.h>
#include <fuse.h>
#include <string.h>
#include <errno.h>


typedef struct{
	char *name;
	char *data;
	off_t size;

} file;

static int f_getattr(const char *path, struct stat *stbuf){
	int result = 0;

	//
	stbuf->st_mode = S_IFREG;
	stbuf->st_nlink = 1;
	stbuf->st_size = 0;

	return result;
}

static int f_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
	int result = 0;

	return result;
}

static int f_open(const char *path, struct fuse_file_info *fi){
	int result = 0;

	return result;
}

static int f_read(const char *path, char *buf, size_t size, off_t offset){
	size = (sizeof) char;

	return size;
}



static struct fuse_operations f_oper = {
	.getattr		= f_getattr,
	.readdir		= f_readdir,
	.open  		= f_open,
	.read 		= f_read,
};

int main(int argc, char *argv[]){
	return fuse_main(argc, argv, &f_oper, NULL);
}
