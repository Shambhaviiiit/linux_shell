#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include "pinfo.h"
void call_pinfo(char *token)
{
    int pid,b;
    char timepass[100];
    if (token == NULL)
        pid = getpid();
    
    else
        pid = atoi(token);
    
    me_too:
    int plus=1;
    printf("pid : %d\n", pid);
    if( tcgetpgrp(STDIN_FILENO)<0 )
        plus = 0;

    char buff[PATH_MAX];
    char buf[1000];
    char file_to_open[1000];
    strcpy(file_to_open, "/proc/");
    sprintf(buf, "%d", pid);
    strcat(file_to_open, buf);

    char read_from[1000];
    strcpy(read_from, file_to_open);
    strcat(read_from, "/exe");
    ssize_t len = readlink(read_from, buff, sizeof(buff) - 1);
    if (len != -1)
    {
        buff[len] = '\0';
        printf("executable path : %s \n", buff);
    }

    char mem_from[1000];
    strcpy(mem_from, file_to_open);
    strcat(mem_from, "/statm");
    FILE *mfile = fopen(mem_from, "r");
    fscanf(mfile, "%d", &b);
    printf("%d\n", b);
    // printf("memory: %d\n", b);
    fclose(mfile);

    int a;
    char status;
    
    strcat( file_to_open, "/stat");

    FILE *fd = fopen(file_to_open, "r");
    fscanf(fd, "%d %s %c", &a, timepass, &status);
    printf("process status : %c", status);

    if( plus ==  1 )
        printf("+\n");
    else
        printf("\n");

    fclose(fd);
    return;
}