#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include "time.h"
#include "variables.h"
#include "ls.h"

char *month(int a)
{
    if (a == 1)
        return "Jan";

    if (a == 2)
        return "Feb";

    if (a == 3)
        return "Mar";

    if (a == 4)
        return "Apr";

    if (a == 5)
        return "May";

    if (a == 6)
        return "Jun";

    if (a == 7)
        return "Jul";

    if (a == 8)
        return "Aug";

    if (a == 9)
        return "Sep";

    if (a == 10)
        return "Oct";

    if (a == 11)
        return "Nov";

    if (a == 12)
        return "Dec";
}

void permissions(mode_t x)
{
    if (x & S_IFDIR)
        printf("d");

    else
        printf("-");

    if (x & S_IRUSR)
        printf("r");

    else
        printf("-");

    if (x & S_IWUSR)
        printf("w");

    else
        printf("-");

    if (x & S_IXUSR)
        printf("x");

    else
        printf("-");

    if (x & S_IRGRP)
        printf("r");

    else
        printf("-");

    if (x & S_IWGRP)
        printf("w");

    else
        printf("-");

    if (x & S_IXGRP)
        printf("x");

    else
        printf("-");

    if (x & S_IROTH)
        printf("r");

    else
        printf("-");

    if (x & S_IWOTH)
        printf("w");

    else
        printf("-");

    if (x & S_IXOTH)
        printf("x");

    else
        printf("-");

    printf(" ");
}

void ls_only(char *files[500], int index, char path[500], int flag)
{
    for (int i = 0; i < index; i++)
    {
        char *isme;
        isme = strdup(path);
        struct stat statbuf;
        strcat(isme, "/");
        strcat(isme, files[i]);
        stat(isme, &statbuf);
        if (statbuf.st_mode & S_IFDIR)
        {
            if (flag == 1)
                printf("\033[;34m%s  \033[0m", files[i]);

            else if (strncmp(files[i], ".", 1) != 0)
                printf("\033[;34m%s  \033[0m", files[i]);
        }
        else
        {
            if (flag == 1)
                printf("\033[;37m%s  \033[0m", files[i]);

            else if (strncmp(files[i], ".", 1) != 0)
                printf("\033[;37m%s  \033[0m", files[i]);
        }
    }
    printf("\n");
}

void ls_l(char *files[500], int index, char path[500], int flag)
{
    for (int i = 0; i < index; i++)
    {
        if (flag == 0 && strncmp(files[i], ".", 1) == 0)
            continue;

        char *isme;
        isme = strdup(path);
        struct stat statbuf;
        strcat(isme, "/");
        isme[strlen(isme)] = '\0';
        strcat(isme, files[i]);
        stat(isme, &statbuf);
        permissions(statbuf.st_mode);
        printf("  %ld  ", statbuf.st_nlink);

        struct passwd *pw = getpwuid(statbuf.st_uid);
        struct group  *gr = getgrgid(statbuf.st_gid);

        printf("%s %s \t", pw->pw_name, gr->gr_name);

        printf("%ld \t", statbuf.st_size);
        struct tm *time_here = localtime(&statbuf.st_mtim);
        printf("%s \t", month(time_here->tm_mon + 1));
        printf("%d \t", time_here->tm_mday);
        printf("%d:", time_here->tm_hour);
        printf("%d \t", time_here->tm_min);

        if (statbuf.st_mode & S_IFDIR)
        {
            if (flag == 1)
                printf("\033[;34m%s\n\033[0m", files[i]);

            else if (strncmp(files[i], ".", 1) != 0)
                printf("\033[;34m%s\n\033[0m", files[i]);
        }
        else
        {
            if (flag == 1)
                printf("\033[;37m%s\n\033[0m", files[i]);

            else if (strncmp(files[i], ".", 1) != 0)
                printf("\033[;37m%s\n\033[0m", files[i]);
        }
    }
}