#include <stddef.h>
#include <sys/types.h>
#define FILE_NAME_LEN 32
#define BLOCK_DATA_SIZE 512 //UNIX BLOCK SIZE
#define MAX_FILES 10000

#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR 011
#define O_CREAT 0100


#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct file_pair_
{
    int inode_id;   
    char name[FILE_NAME_LEN];
}file_pair;


typedef struct superblock_
{
    int num_of_inodes;
    int num_of_blocks;
    int size_of_blocks;
}superblock;

typedef struct inode_
{
    int size;
    int block_list;
    char name[FILE_NAME_LEN];
    int directory_flag; // 1 if directory, 0 if file. 
    int in_root;
    int eof;
}inode;

typedef struct block_
{
    //next_block_id == -1 means block not in use. == -2 means the block is last in the linked list. 
    int next_block_id;
    char data[BLOCK_DATA_SIZE];
    file_pair this_pair;
}block;

//openfile types: file-0, DIR-1, FILE 2
typedef struct myopenfile_
{
    int inode_id;
    int offset;
    int access_flag;
    int type;

}myopenfile;

typedef struct myDIR_
{
    int current_block;
}myDIR;

struct mydirent
{
    char name[FILE_NAME_LEN];
};

int mymount(const char *source, const char *target,const char *filesystemtype, unsigned longmountflags, const void *data);

void mymkfs(int s);

void mount_fs();

void write_fs();

//allocate_file(char[]) returns the inode_id of the given file. if failed to allocate rtrn -1;
int allocate_file(char file_name[FILE_NAME_LEN]);

void print_fs();

//return -1 on fail
int find_empty_inode();

//return -1 on fail
int find_empty_block();

void set_filesize(int fd, int size);

int write_to_file(int fd, int offset, char* data);

//shorten the file 
void shorten_file(int current_block);

int get_block_id(int fd, int offset);

int myopen(const char *pathname, int flags);

int myclose(int myfd);

ssize_t myread(int myfd, void *buf, size_t count);

ssize_t mywrite(int myfd, const void *buf, size_t count);

int find_in_folder(char *name, int folder);

int find_inode_by_name(char *name);

void set_inode_next(int folder,int file);

//lseek() repositions the file offset of the open file description
    //    associated with the file descriptor fd to the argument offset
    //    according to the directive whence as follows:
    // 
    //    SEEK_SET
    //           The file offset is set to offset bytes.
    //    SEEK_CUR
    //           The file offset is set to its current location plus offset
    //           bytes.
    //    SEEK_END
    //           The file offset is set to the size of the file plus offset
    //           bytes.
off_t mylseek(int myfd, off_t offset, int whence);

void print_open_files();

myDIR *myopendir(const char *name);

struct mydirent *myreaddir(myDIR *dirp);

int myclosedir(myDIR *dirp);

void print_directories();

void indent();

int find_inode_by_path(const char *pathname);

void print_open_files();

void print_fs_folders(int inode_id, int depth);



