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
#include "pwd.h"

void pwd__()
{
    char dir[100];
    getcwd(dir, 100);
    printf("%s\n", dir);
}