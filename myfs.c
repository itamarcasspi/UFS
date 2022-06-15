#include "myfs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// typedef struct superblock_
// {
//     int num_of_inodes;
//     int num_of_blocks;
//     int size_of_blocks;
// }superblock;

// typedef struct inode_
// {
//     int size;
//     int block_list;
//     char name[FILE_NAME_LEN];
//     int next_inode; // used for directories; first node is the directory itself; -2 if last in list -1 if not a directory;
// }inode;

// typedef struct block_
// {
//     //next_block_id == -1 means block not in use. == -2 means the block is last in the linked list.
//     int next_block_id;
//     char data[BLOCK_DATA_SIZE];
// }block;

superblock my_sb;
inode *inodes_ptr;
block *blocks_ptr;
myopenfile *opened_files;

//Creates the file system and initalizes it.
void mymkfs(int s)
{
    s -= sizeof(superblock);
    my_sb.num_of_inodes = (int)((s * 0.1) / sizeof(inode)); // 10% of storage space allocated to inodes.
    my_sb.num_of_blocks = (int)((s * 0.9) / sizeof(block)); // 90% of storage space allocated to to blocks(minus my_sb).
    my_sb.size_of_blocks = sizeof(block);

    // init inodes array to empty files
    inodes_ptr = malloc(sizeof(inode) * my_sb.num_of_inodes);
    for (int i = 0; i < my_sb.num_of_inodes; i++)
    {
        inodes_ptr[i].size = -1;
        strcpy(inodes_ptr[i].name, "EMPTYNO"); // leave space for EOF in string
        inodes_ptr[i].block_list = -1;
        inodes_ptr[i].in_root = 1;
        inodes_ptr[i].directory_flag = 0;
        inodes_ptr[i].eof = 0;
    }

    // init blocks array to empty files
    blocks_ptr = malloc(sizeof(block) * my_sb.num_of_blocks);
    for (int i = 0; i < my_sb.num_of_blocks; i++)
    {
        blocks_ptr[i].next_block_id = -1;
        blocks_ptr[i].this_pair.inode_id = -1;
        memset(blocks_ptr[i].data,0,512);
    }

    opened_files = malloc(sizeof(myopenfile) * MAX_FILES);

    for (int i = 0; i < MAX_FILES; i++)
    {
        opened_files[i].inode_id = -1;
        opened_files[i].offset = 0;
        opened_files[i].access_flag = 0;
    }
}


//Mount file named "target" found in "source".
int mymount(const char *source, const char *target, const char *filesystemtype, unsigned longmountflags, const void *data)
{
    FILE *file_ptr;
    char *path = malloc(strlen(source)+strlen(target));
    strcat(path,source);
    if(strlen(source)!=1)
    {
        strcat(path,"/");
    }

    strcat(path,target);
    file_ptr = fopen(path,"a");
    // write the superblock
    if (fwrite(&my_sb, sizeof(superblock), 1, file_ptr) < 0)
    {
        perror("error in writing my_sb");
    }

    // write the inodes
    for (int i = 0; i < my_sb.num_of_inodes; i++)
    {
        fwrite(&inodes_ptr[i], sizeof(inode), 1, file_ptr);
    }

    // write the blocks
    for (int i = 0; i < my_sb.num_of_blocks; i++)
    {
        fwrite(&blocks_ptr[i], sizeof(block), 1, file_ptr);
    }

    fclose(file_ptr);
    free(path);
    return 0;
}

//irrelevant
void mount_fs()
{
    FILE *file_ptr;
    file_ptr = fopen("my_fs", "r");
    // write the superblock

    if (fread(&my_sb, sizeof(superblock), 1, file_ptr) < 0)
    {
        perror("error in writing my_sb");
    }

    // write the inodes
    inodes_ptr = malloc(sizeof(inode) * my_sb.num_of_inodes);

    for (int i = 0; i < my_sb.num_of_inodes; i++)
    {
        if (fread(&inodes_ptr[i], sizeof(inode), 1, file_ptr) < 0)
        {
            perror("error in reading inode");
        }
    }

    // write the blocks
    blocks_ptr = malloc(sizeof(block) * my_sb.num_of_blocks);

    for (int i = 0; i < my_sb.num_of_blocks; i++)
    {
        if (fread(&blocks_ptr[i], sizeof(block), 1, file_ptr) < 0)
        {
            perror("error in reading block");
        }
    }

    // init the open file table;
    opened_files = malloc(sizeof(myopenfile) * MAX_FILES);

    for (int i = 0; i < MAX_FILES; i++)
    {
        if (fread(&opened_files[i], sizeof(myopenfile), 1, file_ptr) < 0)
        {
            perror("error in reading block");
        }
    }

    fclose(file_ptr);
}

//irrelevant
void write_fs()
{
    FILE *file_ptr;
    file_ptr = fopen("my_fs", "w+");
    // write the superblock
    if (fwrite(&my_sb, sizeof(superblock), 1, file_ptr) < 0)
    {
        perror("error in writing my_sb");
    }

    // write the inodes
    for (int i = 0; i < my_sb.num_of_inodes; i++)
    {
        fwrite(&inodes_ptr[i], sizeof(inode), 1, file_ptr);
    }

    // write the blocks
    for (int i = 0; i < my_sb.num_of_blocks; i++)
    {
        fwrite(&blocks_ptr[i], sizeof(block), 1, file_ptr);
    }

    fclose(file_ptr);
}
// used for testing
void print_fs()
{
    printf("Superblock info\n");
    printf("\tnum of blocks = %d\n", my_sb.num_of_blocks);
    printf("\tnum of inodes = %d\n", my_sb.num_of_inodes);
    printf("\tsize of blocks= %d\n", my_sb.size_of_blocks);

    printf("Inodes data\n");
    for (int i = 0; i < my_sb.num_of_inodes; i++)
    {
        printf("\t%d File name %s, size = %d, first block in list = %d, in_root = %d, directory = %d, eof = %d\n", i, inodes_ptr[i].name,
               inodes_ptr[i].size, inodes_ptr[i].block_list, inodes_ptr[i].in_root,
               inodes_ptr[i].directory_flag, inodes_ptr[i].eof);
    }

    printf("Blocks data\n");
    for (int i = 0; i < my_sb.num_of_blocks; i++)
    {
        printf("\t %d Block data: [", i);
        for (int j = 0; j < BLOCK_DATA_SIZE; j++)
        {
            printf("%c", blocks_ptr[i].data[j]);
        }
        printf("], next_block = %d\n", blocks_ptr[i].next_block_id);
        if (blocks_ptr[i].this_pair.inode_id != -1)
        {
            printf("\t\t pair(%d,%s)\n", blocks_ptr[i].this_pair.inode_id, blocks_ptr[i].this_pair.name);
        }
    }
}

int allocate_file(char file_name[FILE_NAME_LEN])
{
    int inode_id = find_empty_inode();
    if (inode_id == -1)
    {
        printf("Error in allocate_file(). Could not find empty inode.\n");
        return -1;
    }

    int block_id = find_empty_block();
    if (inode_id == -1)
    {
        printf("Error in allocate_file(). Could not find empty block.\n");

        return -1;
    }

    // claim inode
    inodes_ptr[inode_id].block_list = block_id;
    strcpy(inodes_ptr[inode_id].name, file_name);

    // claim block
    blocks_ptr[block_id].next_block_id = -2;
    // set inode filesize
    inodes_ptr[inode_id].size = BLOCK_DATA_SIZE;
    return inode_id;
}
int find_empty_block()
{
    for (int i = 0; i < my_sb.num_of_blocks; i++)
    {
        if (blocks_ptr[i].next_block_id == -1)
        {
            return i;
        }
    }
    return -1;
}
int find_empty_inode()
{
    for (int i = 0; i < my_sb.num_of_inodes; i++)
    {
        if (inodes_ptr[i].block_list == -1)
        {
            return i;
        }
    }
    return -1;
}
// will add at least size amount of free data to the current inode. (increments of BLOCK_DATA_SIZE);
void set_filesize(int inode_id, int size)
{
    int num_of_blocks = size / BLOCK_DATA_SIZE;
    if (size % BLOCK_DATA_SIZE > 0)
    {
        num_of_blocks++;
    }
    int total_blocks_for_file = num_of_blocks;

    int curr = inodes_ptr[inode_id].block_list;
    num_of_blocks--;
    while (num_of_blocks > 0)
    {
        // if curr is the last block on the list, we need to add more blocks
        if (blocks_ptr[curr].next_block_id == -2)
        {
            int next = find_empty_block();
            if (next == -1)
            {

                return;
            }
            blocks_ptr[curr].next_block_id = next;
            blocks_ptr[next].next_block_id = -2;
            num_of_blocks--;
            curr = blocks_ptr[curr].next_block_id;
        }
        else // we have enough blocks so far:
        {
            num_of_blocks--;
            curr = blocks_ptr[curr].next_block_id;
        }
    }
    // shorten_file(curr);
    // printf("current is %d\n", curr);
    // blocks_ptr[curr].next_block_id = -2;
    if (inodes_ptr[inode_id].size < total_blocks_for_file * BLOCK_DATA_SIZE)
    {
        inodes_ptr[inode_id].size = total_blocks_for_file * BLOCK_DATA_SIZE;
    }
}

void shorten_file(int block_id)
{
    int curr = block_id;
    int next_block = blocks_ptr[block_id].next_block_id;

    while (next_block >= 0)
    {
        blocks_ptr[curr].next_block_id = -1;
        curr = next_block;
        next_block = blocks_ptr[next_block].next_block_id;
    }
    blocks_ptr[curr].next_block_id = -1;
}

// get the relevant block in regards to the desired offset
int get_block_id(int fd, int offset)
{
    if (offset > inodes_ptr[fd].size)
    {
        set_filesize(fd, offset);
    }
    int blocks_to_travel = offset;
    int curr = inodes_ptr[fd].block_list;

    while (blocks_to_travel > BLOCK_DATA_SIZE)
    {
        blocks_to_travel -= BLOCK_DATA_SIZE;
        curr = blocks_ptr[curr].next_block_id;
        if (curr == -1)
        {
            return -1;
        }
    }
    return curr;
}

// #define O_RDONLY 00
// #define O_WRONLY 01
// #define O_RDWR 011
// #define O_CREAT 0100

// returns FD of "myopenfile". access modes: O_RDONLY, O_WRONLY, or O_RDWR. O_CREAT to create new file.
int myopen(const char *pathname, int flags)
{

    int curr_inode = find_inode_by_path(pathname);
    // if file not found && create flag is true
    if (curr_inode == -1 && flags >= O_CREAT)
    {
        int in_root = 1;
        int path_len = strlen(pathname) + 1; //+1 for null terminator
        char path[path_len];
        for (int i = 0; i < path_len; i++)
        {
            path[i] = pathname[i];
        }

        const char delim[2] = "/";
        char *name = " ";
        name = strtok(path, delim);
        char *prev_name = name;

        curr_inode = find_inode_by_name(name);
        int prev_inode = curr_inode;
        while (name != NULL)
        {
            prev_name = name;
            name = strtok(NULL, delim);
            if (name != NULL)
            {
                prev_inode = curr_inode;
                curr_inode = find_in_folder(name, curr_inode);
                in_root = 0;
            }
        }
        if (name == NULL && curr_inode < 0)
        {
            // path is valid but file not found.
            if (prev_inode < 0 && in_root)
            {
                curr_inode = allocate_file(prev_name);
                flags -= O_CREAT;
                inodes_ptr[curr_inode].in_root = in_root;
            }
            else if (prev_inode < 0)
            {
                printf("Error myopen() path failed\n");
                return -1;
            }

            else
            {
                curr_inode = allocate_file(prev_name);
                flags -= O_CREAT;
                inodes_ptr[curr_inode].in_root = in_root;
                set_inode_next(prev_inode, curr_inode);
            }
        }
    }
    // cant myopen() a directory
    if (inodes_ptr[curr_inode].directory_flag == 1)
    {
        return -1;
    }
    int new_fd;
    for (int i = 0; i < MAX_FILES; i++)
    {
        // if inode_id of openfile is -1, meaning its spot in the array is vacant.
        if (opened_files[i].inode_id < 0)
        {
            new_fd = i;
            opened_files[i].inode_id = curr_inode;
            opened_files[i].offset = 0;
            opened_files[i].access_flag = flags;
            break;
        }
    }

    return new_fd;
}
// Closes the opened file using its FD. rtrn 0 on success, rtrn -1 on fail.
int myclose(int myfd)
{
    if (myfd < 0 || myfd > MAX_FILES)
    {
        return -1;
    }
    if (opened_files[myfd].inode_id < 0)
    {
        return -1;
    }
    opened_files[myfd].inode_id = -1;
    opened_files[myfd].offset = 0;
    opened_files[myfd].access_flag = 0;
    return 0;
}

// return amount of bytes read
ssize_t myread(int myfd, void *buf, size_t count)
{
    if (opened_files[myfd].access_flag == 1)
    {
        // printf("Error myread(). access permission denied\n");

        return -1;
    }
    // check if the file is opened.
    if (opened_files[myfd].inode_id < 0)
    {
        // printf("Error myread(). file_opened not found\n");
        return -1;
    }
    if (inodes_ptr[opened_files[myfd].inode_id].size < opened_files[myfd].offset + count)
    {
        int curr_block = get_block_id(opened_files[myfd].inode_id, opened_files[myfd].offset);
        int block_offset = opened_files[myfd].offset % BLOCK_DATA_SIZE;
        char eof_[1] = {'\0'};
        ssize_t i;
        for (i = 0; i < count; i++)
        {
            // buf[i] = (void*)(blocks_ptr[curr_block].data[block_offset]) ;
            memcpy(buf + i, (void *)(eof_), 1);

            if (block_offset == BLOCK_DATA_SIZE - 1)
            {
                block_offset = 0;
                curr_block = blocks_ptr[curr_block].next_block_id;
            }
            else
            {
                block_offset++;
            }
            opened_files[myfd].offset += 1;
        }
        return i;

    }

    int curr_block = get_block_id(opened_files[myfd].inode_id, opened_files[myfd].offset);
    int block_offset = opened_files[myfd].offset % BLOCK_DATA_SIZE;
    ssize_t i;
    for (i = 0; i < count; i++)
    {
        // buf[i] = (void*)(blocks_ptr[curr_block].data[block_offset]) ;
        memcpy(buf + i, (void *)(blocks_ptr[curr_block].data + block_offset), 1);

        if (block_offset == BLOCK_DATA_SIZE - 1)
        {
            block_offset = 0;
            curr_block = blocks_ptr[curr_block].next_block_id;
        }
        else
        {
            block_offset++;
        }
        opened_files[myfd].offset += 1;
    }

    return i;
}
// return amount of bytes written;
ssize_t mywrite(int myfd, const void *buf, size_t count)
{
    if (opened_files[myfd].inode_id < 0)
    {
        // printf("Error myread(). file_opened not found\n");
        return -1;
    }
    // check permissions
    if (opened_files[myfd].access_flag == 0)
    {
        // printf("Error mywrite(). access permission denied\n");

        return -1;
    }
    // check if need to allocate more size to the file:
    set_filesize(opened_files[myfd].inode_id, (int)(opened_files[myfd].offset + count));
    if (opened_files[myfd].offset + count > inodes_ptr[opened_files[myfd].inode_id].eof)
    {
        inodes_ptr[opened_files[myfd].inode_id].eof = opened_files[myfd].offset + count;
    }
    
    int curr_block = get_block_id(opened_files[myfd].inode_id, opened_files[myfd].offset);
    int block_offset = opened_files[myfd].offset % BLOCK_DATA_SIZE;
    ssize_t i;
    for (i = 0; i < count; i++)
    {
        blocks_ptr[curr_block].data[block_offset] = ((char *)buf)[i];
        if (block_offset == BLOCK_DATA_SIZE - 1)
        {
            block_offset = 0;
            curr_block = blocks_ptr[curr_block].next_block_id;
        }
        else
        {
            block_offset++;
        }
        // printf("++offset[%d],",opened_files[myfd].offset);
        opened_files[myfd].offset += 1;
    }
    return i;
}

// rtrn -1 if failed; else rtrn inode num. only root files.
int find_inode_by_name(char *name)
{
    for (int i = 0; i < my_sb.num_of_inodes; i++)
    {
        if (strcmp(inodes_ptr[i].name, name) == 0 && inodes_ptr[i].in_root == 1)
        {
            return i;
        }
    }
    // for (int i = 0; i < my_sb.num_of_inodes; i++)
    // {
    //     if (inodes_ptr[i].name == name)
    //     {
    //         return i;
    //     }
    // }
    return -1;
}

//Helper function. Find the inode id by name, if inside the given folder. rtrn -1 on not found, -2 on not directory;
int find_in_folder(char *name, int folder)
{
    if (inodes_ptr[folder].directory_flag == 0)
    {
        return -2;
    }
    int curr_file = inodes_ptr[folder].block_list; // block representing current file.
    while (curr_file >= 0)
    {
        // printf("curr block %d. name %s. \n",curr_file,blocks_ptr[curr_file].this_pair.name);

        if (strcmp(blocks_ptr[curr_file].this_pair.name, name) == 0)
        {
            return blocks_ptr[curr_file].this_pair.inode_id;
        }
        curr_file = blocks_ptr[curr_file].next_block_id;
    }
    return -1;
}

//Helper function. given a path find the inode_id
int find_inode_by_path(const char *pathname)
{
    int path_len = strlen(pathname) + 1; //+1 for null terminator
    char path[path_len];
    for (int i = 0; i < path_len; i++)
    {
        path[i] = pathname[i];
    }

    const char delim[2] = "/";
    char *name = " ";
    name = strtok(path, delim);

    // first we find the main folder
    int curr_inode = find_inode_by_name(name);
    while (name != NULL)
    {
        // printf("name:%s  inode_id: %d\n", name);
        name = strtok(NULL, delim);
        if (name != NULL && inodes_ptr[curr_inode].directory_flag == 1)
        {
            curr_inode = find_in_folder(name, curr_inode);
        }
    }
    if (curr_inode == -1)
    {
        return -1;
    }

    return curr_inode;
}

//Helper function. Insert 'file' into 'folder'. 
void set_inode_next(int folder, int file)
{
    // find empty block and claim it
    int new_block = find_empty_block();
    blocks_ptr[new_block].next_block_id = -2;

    // create the pair that represent the file info inside the directory
    file_pair new_pair;
    new_pair.inode_id = file;
    strcpy(new_pair.name, inodes_ptr[file].name);
    // add the pair to the block
    blocks_ptr[new_block].this_pair = new_pair;

    // add the block to the folder's list
    int curr_block = inodes_ptr[folder].block_list;
    while (blocks_ptr[curr_block].next_block_id > 0)
    {
        curr_block = blocks_ptr[curr_block].next_block_id;
    }

    blocks_ptr[curr_block].next_block_id = new_block;
    inodes_ptr[file].in_root = 0;

    inodes_ptr[folder].directory_flag = 1;
}


off_t mylseek(int myfd, off_t offset, int whence)
{
    // if not an openfile fd;
    if (opened_files[myfd].inode_id == -1)
    {
        // printf("Error in mylseek(), given fd is not an open file descriptor \n");
        return -1;
    }

    // set_filesize(opened_files[myfd].inode_id,offset);
    // SEEK_SET
    if (whence == 0)
    {
        opened_files[myfd].offset = offset;
    }
    // SEEK_CUR
    else if (whence == 1)
    {
        opened_files[myfd].offset += offset;
    }
    // SEEK_END
    else if (whence == 2)
    {
        opened_files[myfd].offset = inodes_ptr[opened_files[myfd].inode_id].eof + offset;
    }
    // Whence input incorrect
    else
    {
        // printf("Error in mylseek(), whence[%d] input is incorrect \n", whence);
    }

    return opened_files[myfd].offset;
}


// Allocate a directory in the desired path. return -1 on fail.
int allocate_dir(char *path)
{
    const char delim[2] = "/";
    char *name = " ";
    name = strtok(path, delim);
    int curr_directory = find_inode_by_name(name);
    if (inodes_ptr[curr_directory].directory_flag == 0)
    {
        printf("Error. allocate_dir() failed. error 1 ");
        return -1;
    }

    while (name != NULL)
    {
        if (curr_directory == -1)
        {
            printf("Error. allocate_dir() failed. error 2.\n");
            return -1;
        }
        curr_directory = find_in_folder(name, curr_directory);
    }
    return 1;
}

//Print all open files. used mainly for debugging.
void print_open_files()
{
    for (int i = 0; i < 10000; i++)
    {
        if (opened_files[i].inode_id >= 0)
        {
            printf("%d inode_id[%d] , offset [%d], flag [%d]\n", i, opened_files[i].inode_id, opened_files[i].offset, opened_files[i].access_flag);
        }
    }
}

//Creates a myDIR struct object. if directory not found create new one.
myDIR *myopendir(const char *name)
{
    int curr_inode = find_inode_by_path(name);

    if (curr_inode == -1)
    {
        int in_root = 1;
        int path_len = strlen(name) + 1; //+1 for null terminator
        char path[path_len];
        for (int i = 0; i < path_len; i++)
        {
            path[i] = name[i];
        }

        const char delim[2] = "/";
        char *name_ = " ";
        name_ = strtok(path, delim);
        char *prev_name = name_;

        curr_inode = find_inode_by_name(name_);
        int prev_inode = curr_inode;
        while (name_ != NULL)
        {
            prev_name = name_;
            name_ = strtok(NULL, delim);
            if (name_ != NULL)
            {
                prev_inode = curr_inode;
                curr_inode = find_in_folder(name_, curr_inode);
                in_root = 0;
            }
        }
        if (name_ == NULL && curr_inode < 0)
        {
            // path is valid but file not found.
            if (prev_inode < 0 && in_root)
            {
                curr_inode = allocate_file(prev_name);

                inodes_ptr[curr_inode].in_root = in_root;
                inodes_ptr[curr_inode].directory_flag = 1;
            }
            else if (prev_inode < 0)
            {
                printf("Error myopen() path failed\n");
                return NULL;
            }
            else
            {
                curr_inode = allocate_file(prev_name);

                inodes_ptr[curr_inode].in_root = in_root;
                inodes_ptr[curr_inode].directory_flag = 1;
                set_inode_next(prev_inode, curr_inode);
            }
        }
    }
    if (inodes_ptr[curr_inode].directory_flag == 0)
    {
        printf("Error myopendir(), file found is not a directory.\n");
        return NULL;
    }
    myDIR *new_dir = malloc(sizeof(myDIR));
    new_dir->current_block = inodes_ptr[curr_inode].block_list;

    return new_dir;
}

struct mydirent *myreaddir(myDIR *dirp)
{
    dirp->current_block = blocks_ptr[dirp->current_block].next_block_id;
    if (dirp->current_block == -2)
    {
        return NULL;
    }

    struct mydirent *directory = malloc(sizeof(struct mydirent));
    strcpy(directory->name, blocks_ptr[dirp->current_block].this_pair.name);
    return directory;
}

int myclosedir(myDIR *dirp)
{
    free(dirp);
    return 0;
}

//print all the directories/files in the root folder. 
void print_directories()
{
    printf("******Printing files hierarchy******\n\n");
    for (int i = 0; i < my_sb.num_of_inodes; i++)
    {
        if (inodes_ptr[i].in_root && inodes_ptr[i].block_list >= 0)
        {
            print_fs_folders(i, 0);
        }
    }
    printf("\n");
    printf("************************************\n");
}


//recursive printing of all files/directory in a given directory
void print_fs_folders(int inode_id, int depth)
{
    if (inode_id < 0)
    {
        return;
    }
    if (!inodes_ptr[inode_id].directory_flag)
    {
        indent(depth);
        printf("/%s\n", inodes_ptr[inode_id].name);
        return;
    }
    indent(depth);

    printf("/%s\n", inodes_ptr[inode_id].name);

    int curr_block = blocks_ptr[inodes_ptr[inode_id].block_list].next_block_id;
    while (curr_block >= 0)
    {
        // printf("/%s\n",blocks_ptr[curr_block].this_pair.name);
        print_fs_folders(blocks_ptr[curr_block].this_pair.inode_id, depth + 1);
        curr_block = blocks_ptr[curr_block].next_block_id;
    }
}


//Used to format print_directories;
void indent(int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("\t");
    }
}