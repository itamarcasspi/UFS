#include "myfile.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define INT_DIGIT_LENGTH 10
#define FLOAT_LENGTH 64

myFILE *myfopen(const char *pathname, const char *mode)
{
    myFILE *file = malloc(sizeof(myFILE));
    int flag = -1;
    int new_fd;
    if (strcmp("r", mode) == 0)
    {
        new_fd = myopen(pathname, O_RDONLY);

        flag = 0;
    }
    else if (strcmp("w", mode) == 0)
    {
        new_fd = myopen(pathname, O_WRONLY);

        flag = 1;
    }
    else if (strcmp("a", mode) == 0) // append mode.  create file if not found.
    {
        new_fd = myopen(pathname, O_WRONLY | O_CREAT);
        mylseek(new_fd, 0, SEEK_END);
        flag = 2;
    }
    else if (strcmp("r+", mode) == 0) // read and write. file must exist.
    {
        new_fd = myopen(pathname, O_RDWR);
        flag = 3;
    }
    else if (strcmp("w+", mode) == 0) // read and write, + create empty file.
    {
        new_fd = myopen(pathname, O_RDWR | O_CREAT);

        flag = 4;
    }
    else if (strcmp("a+", mode) == 0) // reading and appending.
    {
        new_fd = myopen(pathname, O_RDWR | O_CREAT);
        mylseek(new_fd, 0, SEEK_END);
        flag = 5;
    }
    else
    {
        printf("Error in myfopen()\n");
        return NULL;
    }

    file->restrictions = flag;
    file->fd = new_fd;
    return file;
}

int myfclose(myFILE *stream)
{
    myclose(stream->fd);
    free(stream);
    return 0;
}

size_t myfread(void *ptr, size_t size, size_t nmemb, myFILE *stream)
{
    return myread(stream->fd, ptr, size * nmemb);
}

size_t myfwrite(const void *ptr, size_t size, size_t nmemb, myFILE *stream)
{
    if (stream->restrictions == 0) // read only || append only
    {
        return -1;
    }
    return mywrite(stream->fd, ptr, nmemb * size);
}

int myfseek(myFILE *stream, long offset, int whence)
{
    return mylseek(stream->fd, offset, whence);
}

int myfscanf(myFILE *stream, const char *format, ...)
{
    int format_len = strlen(format) + 1; //+1 for null terminator
    char curr_format[format_len];
    for (int i = 0; i < format_len; i++)
    {
        curr_format[i] = format[i];
    }
    int count = 0;
    va_list arguments;
    va_start(arguments, format);
    char *delim = " ";
    char *token = " ";
    token = strtok(curr_format, delim);
    while (token != NULL)
    {
        printf("CURR TOKEN %s\n", token);

        if (strcmp(token, "%s") == 0)
        {
            printf("FOUND STRING\n");
            char curr[2] = {'\0', '\0'};
            char space[2] = {' ', '\0'};
            char newline[2] = {'\n', '\0'};
            char tab[2] = {'\t', '\0'};
            char eof[1] = {'\0'};

            char *var = va_arg(arguments, char *);
            // myfread(curr,1,1,stream);
            while (strcmp(curr, space) != 0 || strcmp(curr, newline) != 0 || strcmp(curr, tab) != 0)
            {
                if (myfread(curr, 1, 1, stream))
                {
                    if(strcmp(curr, space) == 0)
                    {
                        strcat(var, eof);
                        break;
                    }
                    printf("curr[%s]\n", curr);
                    strcat(var, curr);

                    if ((int)curr[0] == 0)
                    {
                        break;
                    }
                    count++;
                }
                else
                {
                    printf("read fail\n");
                    break;
                }
            }
        }
        else if (strcmp(token, "%d") == 0)
        {
            char curr[2] = {'\0', '\0'};
            char space[2] = {' ', '\0'};
            char newline[2] = {'\n', '\0'};
            char tab[2] = {'\t', '\0'};

            int *var = va_arg(arguments, int *);
            int number_read = 0;
            myfread(curr, 1, 1, stream);
            char buffer[INT_DIGIT_LENGTH+2] = {0};//+minus sign + null termination
            if (!isdigit(curr[0]))
            {
                if (strcmp(curr, "-") == 0)
                {
                    buffer[0] = '-';
                    myfread(curr, 1, 1, stream);
                }
                else
                {
                    break;
                }
            }
            printf("BUFFER[%s]\n", buffer);

            while (strcmp(curr, space) != 0 || strcmp(curr, newline) != 0 || strcmp(curr, tab) != 0)
            {
                if (!isdigit(curr[0]))
                {
                    if (number_read)
                    {

                        printf("READING %d\n", atoi(buffer));
                        *var = atoi(buffer);
                        count++;
                        mylseek(stream->fd, -1, SEEK_CUR);
                    }
                    break;
                }
                number_read = 1;
                strcat(buffer, curr);
                printf("BUFFER[%s]\n", buffer);
                if (myfread(curr, 1, 1, stream))
                {
                    count++;
                }
            }
        }
        else if (strcmp(token, "%f") == 0)
        {
            char curr[2] = {'\0', '\0'};
            char space[2] = {' ', '\0'};
            char newline[2] = {'\n', '\0'};
            char tab[2] = {'\t', '\0'};

            float *var = va_arg(arguments, float *);
            int number_read = 0;
            myfread(curr, 1, 1, stream);
            char buffer[INT_DIGIT_LENGTH+2] = {0};
            if (!isdigit(curr[0]))
            {
                if (strcmp(curr, "-") == 0)
                {
                    buffer[0] = '-';
                    myfread(curr, 1, 1, stream);
                }
                else
                {
                    break;
                }
            }
            printf("BUFFER[%s]\n", buffer);

            while (strcmp(curr, space) != 0 || strcmp(curr, newline) != 0 || strcmp(curr, tab) != 0)
            {
                if (!isdigit(curr[0]))
                {
                    if (number_read)
                    {
                        printf("READING %f\n", atof(buffer));
                        *var = atoi(buffer);
                        count++;
                        mylseek(stream->fd, -1, SEEK_CUR);
                    }
                    break;
                }
                number_read = 1;
                strcat(buffer, curr);
                printf("BUFFER[%s]\n", buffer);
                if (myfread(curr, 1, 1, stream))
                {
                    count++;
                }
            }
        }

        else
        {
            break;
        }
        token = strtok(NULL, delim);
    }
    va_end(arguments);
    return count;
}

int myfprintf(myFILE *stream, const char *format, ...)
{
    int format_len = strlen(format) + 1; //+1 for null terminator
    char curr_format[format_len];
    for (int i = 0; i < format_len; i++)
    {
        curr_format[i] = format[i];
    }
    int count = 0;
    va_list arguments;
    va_start(arguments, format);
    char *delim = " ";
    char *token = " ";
    token = strtok(curr_format, delim);
    while(token != NULL)
    {
        printf("CURR %s\n",token);
        if(strcmp(token, "%s") == 0)
        {
            char *str = va_arg(arguments,char*);
            
            for (int i = 0; str[i]!= '\0'; i++)
            {
                printf("string buffering [%c]\n",str[i]);
                myfwrite(str+i,1,1,stream);
            }
            count++;
        }
        else if (strcmp(token, "%d") == 0)
        {
            int num = va_arg(arguments,int);
            char buffer[INT_DIGIT_LENGTH + 1];
            memset(buffer,0,FLOAT_LENGTH+1);
            sprintf(buffer,"%d",num);
            for (int i = 0; i<INT_DIGIT_LENGTH+1 && buffer[i] != '\0'; i++)
            {
                myfwrite(buffer+i,1,1,stream);
            }
            count++;
        }
        else if (strcmp(token, "%f") == 0)
        {
            double num = va_arg(arguments,double);
            printf("FLOAT %f\n",num);
            char buffer[FLOAT_LENGTH + 1];
            memset(buffer,0,FLOAT_LENGTH+1);
            sprintf(buffer,"%f",num);
            printf("BUFFER %s\n",buffer);
            for (int i = 0; i<FLOAT_LENGTH+1 && buffer[i] != '\0'; i++)
            {
                printf("buffering[%s]\n",buffer+i);
                myfwrite(buffer+i,1,1,stream);
            }
            count++;
        }

        token = strtok(NULL, delim);
    }

    return count;
}