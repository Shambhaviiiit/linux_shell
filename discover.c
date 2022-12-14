#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include "discover.h"
#include "variables.h"

void discover_this(char *path, char *toprint, char *first_wala, int flagd, int flagf)
{
    char *file;
    char toprint2[1000];
    DIR *dd = opendir(path);
    if( first_wala != NULL && flagd == 1 && (access(path, F_OK) == 0 || dd != NULL ))
        printf("%s\n", first_wala);
    if (dd == NULL)
    {
        if( first_wala != NULL)
            printf("error: no such file or directory\n");
        return;
    }
    struct dirent *str_dir;
    str_dir = readdir(dd);
    if (str_dir == NULL)
    {
        printf("error discovering\n");
        return;
    }
    
    while (str_dir != NULL)
    {
        file = strdup(str_dir->d_name);
        if (file == NULL)
        {
            printf("error in file name\n");
            return;
        }
        if( file[0] == '.' )
            goto direct;

        char name[100];
        strcpy(name, path);
        strcat(name, "/");
        strcat(name, file);
        
        // check if it is a directory or a file

        struct stat statbuf;
        stat(name, &statbuf);
        if( statbuf.st_mode & S_IFDIR )
        {
            if( flagd == 1 )
                printf("%s/%s\n", toprint, str_dir->d_name);
        }
        else if( flagf == 1 )
            printf("%s/%s\n", toprint, str_dir->d_name);

        strcpy(toprint2, toprint);
        strcat(toprint2, "/");
        strcat(toprint2, str_dir->d_name);
        
        if (str_dir->d_name[0] != '.')
            discover_this(name, toprint2, NULL, flagd, flagf);

        direct:    
        str_dir = readdir(dd);
    }
    closedir(dd);
    return;
}