#define FUSE_USE_VERSION	26

#include <stdio.h>
#include <fuse.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define TYPE_FILE 1
#define TYPE_DIR 0


typedef struct{
	char *name;
	char *data;
	char type;
	off_t size;
	mode_t mode;
	time_t atime;
	time_t mtime;
	time_t ctime;
	uid_t uid;
	gid_t gid;

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
				printf("#getfile: %s, %d\n", name, i);
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
		stbuf->st_ctime = _file->ctime;
		stbuf->st_atime = _file->atime;
		stbuf->st_mtime = _file->mtime;
		stbuf->st_uid = _file->uid;
		stbuf->st_gid = _file->gid;
		printf("#getattr: %s %d\n", _file->name, _file->size);
	} else
		result = -ENOENT;
	
	return result;
}

static int f_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
	printf("@readdir: %s\n", path);
	int result = 0;
	int i;
	// file *_file = getfile(path);

	if (strcmp(path, "/") != 0)
		return -ENOENT;
	else{
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);
		for(i=0; i<file_count; i++){
			filler(buf, file_list[i]->name+1, NULL, 0);
			printf("#filler: %s\n", file_list[i]->name);
		}
	}
	return result;
}

static int f_open(const char *path, struct fuse_file_info *fi){
	printf("@open: %s\n", path);
	int result = 0;
	file *_file = getfile(path);
	uid_t uid = fuse_get_context()->uid;
	uid_t gid = fuse_get_context()->gid;

	if(_file != NULL){
		if(uid != _file->uid || gid != _file->gid){
			result = -EACCES;
		}
	}else{
		result = -ENOENT;
	}
	fi->fh = _file;
	
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

static int f_create(const char *path, mode_t mode, struct fuse_file_info *fi){
	printf("@create: %s\n", path);
	int result = 0;
	file *_file = (file*)malloc(sizeof(file));
	if(_file != NULL){
		file_list[file_count] = _file;
		file_count++;

		_file->type = TYPE_FILE;
		_file->name = strdup(path);
		_file->data = NULL;
		_file->size = 0;
		time(&_file->ctime);
		time(&_file->atime);
		time(&_file->mtime);
		printf("time is %d\n", _file->ctime);
		printf("created file is %s\n", _file->name);
		_file->uid = fuse_get_context()->uid;
		_file->gid = fuse_get_context()->gid;
		_file->mode = mode;
		fi->fh = _file;
	}
	

	return result;

}

static int f_unlink(const char *path){
	printf("@unlink: %s\n", path);
	int result = 0;

	return result;
}

static int f_truncate(const char *path, off_t size){
	printf("@truncate: %s\n", path);
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
	.unlink 		= f_unlink,
	.truncate 	= f_truncate

};

int main(int argc, char *argv[]){
	return fuse_main(argc, argv, &f_oper, NULL);
}
