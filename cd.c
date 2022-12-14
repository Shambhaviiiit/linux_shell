#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include "cd.h"
#include "print.h"
#include "variables.h"
char prev_dir[100];
int cdir(char *token, char *home_dir, int flag)       // here token is token[1] from main
{
    // if (token[1] == NULL)
    //     goto tilda;
    if (strcmp(token, "-") == 0)
    {
        if( flag==0 )
            printf("%s\n", prev_dir);

        if (chdir(prev_dir) == -1)
        {
            printf("error changing directory\n");
            // goto here;
            // print_dir();
            return 1;
        }
        return 0;
    }
    getcwd(prev_dir, 100);
    if (strcmp(token, ".") == 0)
    {
        // print_dir();
        return 1;
    }    

    else if (strcmp(token, "~") == 0 || strcmp(token, "~/") == 0)
    {
    tilda:
        chdir(home_dir);
        // print_dir();
        return 1;
    }
    else if (strcmp(token, "..") == 0)
    {
        char dir[100];
        getcwd(dir, 100);

        int dir_len = strlen(dir);
        int this;
        for (int i = dir_len - 1; i >= 0; i--)
        {
            if (dir[i] == '/')
            {
                this = i;
                break;
            }
        }
        dir[this] = '\0';
        chdir(dir);
        return 0;
    }
    char new_dir[100];
    getcwd(new_dir, 100);
    int l1 = strlen(new_dir);
    new_dir[l1] = '/';
    new_dir[l1 + 1] = '\0';
    strcat(new_dir, token);
    int x = chdir(new_dir);
    if (x == -1 && flag == 0 )
    {
        printf("error: directory not found in %s\n", token);
        return -1;
    }
    return 0;
}