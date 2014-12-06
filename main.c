#define FUSE_USE_VERSION	26

#include <stdio.h>
#include <fuse.h>
#include <string.h>
#include <errno.h>


typedef struct{
	char *name;
	char *data;
	off_t size;
	mode_t mode;

} file;

file *file_list[200];
int file_count = 0;

file *getfile(const char *name){
	file *target_file = NULL;
	int i;
	if(name!=NULL){
		for(i=0; i<file_count; i++){
			file *iter_file = file_list[i];
			if(strcmp(iter_file->name, name)==0){
				target_file = iter_file;
			}
		}
	}
	return target_file;
}

static int f_getattr(const char *path, struct stat *stbuf){
	printf("@getattr: %s\n",path);
	int result = 0;
	memset(stbuf, 0, sizeof (stbuf));

	file *_file = getfile(path);
	
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (_file != NULL) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = _file->size;
	} else
		result = -ENOENT;
	
	return result;
}

static int f_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
	printf("@readdir: %s\n", path);
	int result = 0;
	int i;
	file *_file = getfile(path);

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	if(_file != NULL){	
		
		for(i=0; i<file_count; i++){
			filler(buf, file_list[i]->name, NULL, 0);
		}	
	}
	

	return result;
}

static int f_open(const char *path, struct fuse_file_info *fi){
	printf("@open: %s\n", path);
	int result = 0;

	return result;
}

static int f_read(const char *path, char *buf, size_t size, off_t offset){
	printf("@read: %s\n", path);
	size = sizeof (char);

	return size;
}

static int f_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	printf("@write: %s\n", path);
	return size;
}

static int f_create(const char *path, mode_t mode, struct  fuse_file_info *fi){
	printf("@create: %s\n", path);
	int result = 0;

	return result;

}

static int f_unlink(const char *path){
	printf("@unlink: %s\n", path);
	int result = 0;

	return result;
}



static struct fuse_operations f_oper = {
	.getattr		= f_getattr,
	.readdir		= f_readdir,
	.open  		= f_open,
	.read 		= f_read,
	.write 		= f_write,
	.create 		= f_create,
	.unlink 		= f_unlink

};

int main(int argc, char *argv[]){
	return fuse_main(argc, argv, &f_oper, NULL);
}
