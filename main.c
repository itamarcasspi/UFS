#include "myfs.h"
#include <stdio.h>
#include <fcntl.h>
// typedef struct file_pair_
// {
//     int inode_id;
//     char *name[64];
// }file_pair_;



int main()
{
    mymkfs(10000);
    // write_fs();
    // mount_fs();
    // int fd1 = allocate_file("first");
    // int fd2 = allocate_file("second");
    // int fd3 = allocate_file("third");
    // int fd4 = allocate_file("file");

    // set_inode_next(fd1,fd2);
    // set_inode_next(fd1,fd3);
    // set_inode_next(fd3,fd4);
    

    // set_filesize(fd,4000);
    // print_fs();
    // printf("in folder? %d\n",find_in_folder("file",0));
    // myopen("first/second",0);
    // int file_fd = myopen("third/file",0);
    // char buffer[6] = {'a','b','c','d','e','\0'};
    // mylseek(file_fd,509,SEEK_SET);
    // mywrite(file_fd,buffer,6);

    // mylseek(file_fd,509,SEEK_SET);
    // print_fs();

    // // print_open_files();

    // char other_buffer[6];
    // myread(file_fd,other_buffer,6);
    // printf("The other buffer read %s\n",other_buffer);

    // set_inode_next(fd3,fd2);
    // set_inode_next(fd3,fd1);
    // print_fs();
    // myDIR *third_DIR = myopendir("third");
    // struct mydirent *curr_dir;
    // curr_dir = myreaddir(third_DIR);
    // while(curr_dir != NULL)
    // {
    //     printf("Current file name = %s\n",curr_dir->name);
    //     curr_dir = myreaddir(third_DIR);

    // }
    
    // myopen("home/hey/world/file.txt",4);
    // set_filesize(fd,2000);
    // print_fs();

    //tests for get_block_id():
    // printf("Block id after offset(%d) is %d\n",4001,get_block_id(fd,4001));
    // printf("Block id after offset(%d) is %d\n",512,get_block_id(fd,512));
    // printf("Block id after offset(%d) is %d\n",500,get_block_id(fd,500));
    // printf("Block id after offset(%d) is %d\n",516,get_block_id(fd,516));
    // printf("Block id after offset(%d) is %d\n",2,get_block_id(fd,2));
    // printf("Block id after offset(%d) is %d\n",0,get_block_id(fd,0));
    // printf("Block id after offset(%d) is %d\n",2300,get_block_id(fd,2300));


    // myopendir("Folder_1");
    myopendir("Folder_1");
    myopendir("Folder_1/Folder_2");

    // myopen("Folder_1",O_CREAT);

    myopen("Folder_1/file",O_RDWR | O_CREAT);
    if(!myopendir("Folder_1/file"))
    {
        printf("File is not a directory\n");
    }
    // myopendir("Folder_1/Folder_3");

    // myopen("Folder_1/Folder_3/file2",O_RDWR | O_CREAT);
    // myopen("Folder_1/Folder_2/file3",O_RDWR | O_CREAT);
    // myopen("file_in_root",O_RDWR | O_CREAT);
    int filetxt = myopen("Folder_1/Folder_2/file.txt",O_RDWR | O_CREAT);
    // mylseek(filetxt,510,SEEK_SET);

    char buffer[6] = {'a','b','c','d','e','\0'};
    mywrite(filetxt,buffer,6);
    mylseek(filetxt,0,SEEK_SET);
    char new_buffer[6];
    myread(filetxt,new_buffer,6);

    // printf("$$%s$$\n",new_buffer);



    // char buffer1[2] = {'b','\0'};
    // mywrite(filetxt,buffer1,1);

    // mylseek(filetxt,0,SEEK_END);
    // char buffer2[2] = {'f','\0'};
    // mywrite(filetxt,buffer2,2);


    // char newbuffer[12];
    // myread(filetxt,newbuffer,12);
    // printf("%s\n",newbuffer);
    // myopendir("Folder1/Folder2");
    print_fs(); 
    print_open_files();
    mymount("/home","swoop", " " ,2 ," " );
    // print_directories();





}