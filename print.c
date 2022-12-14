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
#include "variables.h"
#include "print.h"
extern int fd;
void print_dir()
{
    dup2(fd, 1);
    char dir[100];
    getcwd(dir, 100);

    int dir_len = strlen(dir);
    if (dir_len < len)
        printf("\033[;36m%c%s%s%s>\033[0m", '<', user, sysname, dir);

    else if (dir_len > len)
        printf("\033[;36m%c%s%s~%s>\033[0m", '<', user, sysname, &dir[len]);

    else
        printf("\033[;36m%c%s%s~>\033[0m", '<', user, sysname);
}
