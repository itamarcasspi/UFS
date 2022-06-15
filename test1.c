#include "myfile.h"
#include <stdio.h>
#include <stdlib.h>
int main()
{
    mymkfs(10000);
    myFILE *file = myfopen("File1","r");
    char buffer[6] = {'-','2','3','4','f'};
    char new_buffer[4]  = {'e',' ','6','\0'};
    myfwrite(buffer,5,1,file);
    // myfclose(file);
    myFILE *file2 = myfopen("File1","a");
    myfwrite(new_buffer,4,1,file2);
    
    // printf("%ld\n",myfread(new_buffer,6,1,file));
    // printf("%s\n",new_buffer);
    myfseek(file2,0,SEEK_SET);
    print_fs();

    char str[100];
    // myfscanf(file2,"%s",str);
    // printf("%s\n",str);

    int num;
    float num2;
    myfscanf(file2,"%s %f",str,&num2);
    // myfscanf(file2,"%s",str);
    // myfseek(file2,0,SEEK_SET);

    printf("%s,%f\n",str,num2);
    // print_open_files();
    myfprintf(file2,"%s %s %d %s %f","Hey"," Shmuck",42," Nice ",700000000000000.5);
    myFILE *file3 = myfopen("File8","a");
    myfprintf(file3,"%s %s %d %s %f %d %s","Hey"," FSAFSAss",42," Nice ",700000000000000.5,42,"$Big World");
    
    print_fs();



    return 0;
}