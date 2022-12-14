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
#include <errno.h>
#include "pwd.h"
#include "cd.h"
#include "variables.h"
#include "print.h"
#include "pinfo.h"
#include "history.h"
#include "discover.h"
#include "ls.h"

//  ls -l mei colours baaki hain

int len;
char *user;
char sysname[100];
int ulen;
int bg_arr[1000];
int bg_i;
int x;

int compare(const void *p1, const void *q1)
{
    return strcasecmp(*(const char **)p1, *(const char **)q1);
}
void signal_handler(int signum)
{
    for (int i = 0; i < bg_i; i++)
    {
        int status;
        if (bg_arr[i] != 0 && waitpid(bg_arr[i], &status, WNOHANG) != 0)
        {
            if (WIFEXITED(status))
                printf("pid %d exited normally\n", bg_arr[i]);

            else
                printf("pid %d exited abnormally\n", bg_arr[i]);

            bg_arr[i] = 0;
        }
    }
}
int main()
{
    char *history_arr[1000];
    int hist_ind=0;
    hist_ind = history_read(history_arr);

    char home_dir[100];
    getcwd(home_dir, 100);
    user = getlogin();
    ulen = strlen(user);
    user[ulen] = '@';
    user[ulen + 1] = '\0';

    gethostname(sysname, 100);
    int slen = strlen(sysname);
    sysname[slen] = ':';
    sysname[slen + 1] = '\0';

    len = strlen(home_dir);
    printf("\033[;36m%c%s%s~>\033[0m", '<', user, sysname);

    signal(SIGCHLD, signal_handler);
    while (1)
    {
        char input[100];
        gets(input);
        bg_i = 0;
        char *commands[10];
        char delimiters[] = ";\n";
        commands[0] = strtok(input, delimiters);
        if (commands[0] == NULL)
            goto here;

        int cmd_no = 1;
        while (commands[cmd_no - 1] != NULL)
        {
            commands[cmd_no] = strtok(NULL, delimiters);
            cmd_no++;
        }
        cmd_no--;

        for (int c = 0; c < cmd_no; c++)
        {
            int number = 0, it = strlen(commands[c]);
            // printf("%d  \n", it);
            char *parts[10];
            int not__ = 0;
            it--;
            while (it >= 0)
            {
                if (commands[c][it] == ' ')
                {
                    it--;
                    continue;
                }
                else if (commands[c][it] == '&')
                {
                    number = 1;
                    break;
                }
                else
                {
                    break;
                }
                it--;
            }
            if (it == -1)
            {
                not__ = 1;
            }
            // if (commands[c][strlen(commands[c]) - 1] == '&' )
            //     number = 1;
            // printf("n%d\n", number);

            // .....

            if( hist_ind > 0 && strcmp(commands[c], history_arr[hist_ind-1]) != 0)
            {
                history_arr[hist_ind] = (char *)malloc(sizeof(char *)+1);
                strcpy(history_arr[hist_ind], commands[c]);
                hist_ind++;
            }
            else if( hist_ind == 0 )
            {
                history_arr[0] = (char *)malloc(sizeof(char) + 1);
                strcpy(history_arr[0], commands[c]);
                hist_ind++;
            }
            
            // .....
            parts[0] = strtok(commands[c], "&");

            int p_no = 1;
            while (parts[p_no - 1] != NULL)
            {
                parts[p_no] = strtok(NULL, "&");

                p_no++;
            }

            p_no--;
            int p_i = 0;
            // printf("pno%d", p_no);
            while (p_i < p_no)
            {
                char *token[10];
                // printf("%s \n", parts[p_i]);
                token[0] = strtok(parts[p_i], " \t\n");
                if (token[0] == NULL)
                    continue;
                if (strcmp(token[0], "q") == 0)
                {
                    hist_clear(history_arr, hist_ind);
                    return 0;
                }

                int no = 1;

                while (token[no - 1] != NULL)
                {
                    // printf("%s\n", token[no - 1]);
                    token[no] = strtok(NULL, " \t\n");
                    no++;
                }
                no--;

                // if( p_no == 1 && number == 0 )
                //     goto try;

                if ((number == 1) || (number == 0 && p_i != p_no - 1 && p_no > 1))
                {
                    printf("1.Hi\n");
                    int id = fork();
                    if (id > 0)
                    {
                        printf("[%d] %d\n", bg_i + 1, id);
                        bg_arr[bg_i] = id;
                        bg_i++;
                    }

                    if (id == 0)
                    {
                        // printf("here\n");
                        token[no] = NULL;

                        int err = execvp(token[0], token);
                        if (err == -1)
                            printf("error: command not found\n");

                        return 0;
                    }
                    // try:
                }

                else if (strcmp("echo", token[0]) == 0)
                {
                    // printf("hi2\n");
                    if (token[1] == NULL)
                    {
                        printf("\n");
                        goto here;
                    }
                    if (no > 2)
                    {
                        for (int i = 1; i < no - 1; i++)
                            printf("%s ", token[i]);
                    }

                    printf("%s\n", token[no - 1]);
                }

                else if (strcmp("pwd", token[0]) == 0)
                {
                    pwd__();
                }

                else if (strcmp("cd", token[0]) == 0)
                {
                    if (token[1] == NULL)
                        chdir(home_dir);

                    else
                        cdir(token[1], home_dir, 0);
                }

                else if (strcmp(token[0], "ls") == 0)
                {
                    int t = 1, flaga = 0, flagl = 0;
                    char path[500];
                    getcwd(path, 500);
                    if (token[1] == NULL)
                    {
                    back:
                        char *files1[500];

                        DIR *dd = opendir(path);
                        if( dd == NULL )
                            printf("error: no such directory\n");

                        struct dirent *str_dir;
                        str_dir = (struct dirent *)malloc(sizeof(struct dirent*) + 1);
                        str_dir = readdir(dd);
                        int index = 0;
                        while (str_dir != NULL)
                        {
                            files1[index] = (char *)malloc(1000 * sizeof(char*));
                            strcpy(files1[index], str_dir->d_name);
                            index++;
                            str_dir = readdir(dd);
                        }
                        closedir(dd);
                        qsort(files1, index, sizeof(char *), compare);

                        if (flagl == 1)
                            ls_l(files1, index, path, flaga);

                        else
                            ls_only(files1, index, path, flaga);

                        // else ls_only(files1, index, path, 0);
                        goto here;
                    }

                    char *folders[500];
                    char *files[500];
                    int f_index = 0, file_index = 0;
                    while (token[t] != NULL)
                    {
                        if (strcmp(token[t], "-a") == 0)
                            flaga = 1;

                        else if (strcmp(token[t], "-l") == 0)
                            flagl = 1;

                        else if (strcmp(token[t], "-al") == 0 || strcmp(token[1], "-la") == 0)
                        {
                            flaga = 1;
                            flagl = 1;
                        }
                        else
                        {
                            if (cdir(token[t], home_dir, 1) != -1)
                            {
                                folders[f_index] = strdup(token[t]);
                                f_index++;
                            }
                            else
                            {
                                files[file_index] = strdup(token[t]);
                                file_index++;
                            }
                        }
                        t++;
                    }
                    qsort(folders, f_index, sizeof(char *), compare);
                    qsort(files, file_index, sizeof(char *), compare);

                    for (int i = 0; i < file_index; i++)
                        printf("%s  ", files[i]);

                    if (file_index != 0)
                        printf("\n");

                    for (int i = 0; i < f_index; i++)
                        printf("%s  ", folders[i]);

                    if (f_index != 0)
                        printf("\n");

                    if (flagl == 1)
                        ls_l(files, file_index, path, flaga);

                    else
                        ls_only(files, file_index, path, flaga);

                    for (int i = 0; i < f_index; i++)
                    {
                        char path0[500];
                        getcwd(path0, 500);
                        // printf(": %s\n", folders[i]);
                        cdir(folders[i], home_dir, 1);
                        char *files[500];
                        char path[500];
                        getcwd(path, 500);
                        DIR *dd = opendir(path);
                        struct dirent *str_dir;
                        str_dir = (struct dirent *)malloc(sizeof(struct dirent));
                        str_dir = readdir(dd);
                        int index = 0;
                        while (str_dir != NULL)
                        {
                            files[index] = (char *)malloc(strlen(str_dir->d_name) * sizeof(char));
                            strcpy(files[index], str_dir->d_name);
                            index++;
                            str_dir = readdir(dd);
                        }
                        qsort(files, index, sizeof(char *), compare);

                        if (flagl == 1)
                            ls_l(files, index, path, flaga);

                        else
                            ls_only(files, index, path, flaga);

                        // cdir("-", home_dir, 1);
                        cdir(path0, home_dir, 1);
                        closedir(dd);
                    }
                    if (f_index == 0 && file_index == 0)
                        goto back;

                    for( int i=0; i<index; i++ )
                    {
                        free(files[i]);
                    }    
                }

                else if (strcmp(token[0], "pinfo") == 0)
                {
                    call_pinfo(token[1]);
                }

                else if (strcmp(token[0], "discover") == 0)
                {
                    if (token[1] == NULL)
                    {
                        char buf[1000];
                        char temp[1000] = ".";
                        discover_this(getcwd(buf, 1000), temp, ".", 1, 1);
                    }

                    else
                    {
                        char *store[1000];
                        int t = 1, flagd = 0, flagf = 0, flag_check = 0, st_index = 0;
                        while (token[t] != NULL)
                        {
                            if (strcmp(token[t], "-d") == 0)
                            {
                                flagd = 1;
                                t++;
                                continue;
                            }

                            else if (strcmp(token[t], "-f") == 0)
                            {
                                flagf = 1;
                                t++;
                                continue;
                            }

                            else
                            {
                                store[st_index] = (char *)malloc(sizeof(char *));
                                strcpy(store[st_index], token[t]);
                                st_index++;
                                t++;
                            }
                        }
                        if (flagd == 0 && flagf == 0)
                        {
                            flagd = 1;
                            flagf = 1;
                        }
                        if (st_index == 0)
                        {
                            char buf[1000];
                            char temp[1000] = ".";
                            if (flag_check == 0)
                                discover_this(getcwd(buf, 1000), temp, ".", flagd, flagf);
                        }
                        t = 0;
                        while (t < st_index)
                        {

                            char buf[1000];
                            getcwd(buf, 1000);
                            char temp[1000];
                            char first_wala[1000];

                            if( strcmp(store[t], "..") == 0 )
                            {
                                strcpy(first_wala, "..");
                                strcpy(temp, "..");
                            }

                            else if (strcmp(store[t], ".") == 0 || strcmp(store[t], "./") == 0)
                            {
                                strcpy(first_wala, ".");
                                strcpy(temp, ".");
                            }

                            else if (strncmp(store[t], "./", 2) != 0) // only hello case
                            {
                                strcpy(temp, "./");
                                strcat(temp, store[t]);
                                strcpy(first_wala, "./");
                                strcat(first_wala, store[t]);
                            }

                            else // ./hello case
                            {
                                strcpy(first_wala, store[t]);
                                strcpy(temp, store[t]);
                            }

                            strcat(buf, "/");
                            strcat(buf, store[t]);
                            // printf("args: %s, %s");
                            discover_this(buf, temp, first_wala, flagd, flagf);
                            flag_check = 1;
                            t++;
                        }
                    }
                }

                else if (strcmp(token[0], "history") == 0)
                {
                    char *ret[100];
                    // if (hist_ind == 0)
                    // {
                    //     int last = history_call(ret, 10);
                    //     if (last == -1)
                    //     {
                    //         // hist_clear(history_arr, hist_ind);
                    //         // hist_ind=0;
                    //         // last = history_call(ret);

                    //         for (int i = 0; i < hist_ind; i++)
                    //             printf("%s\n", history_arr[i]);
                    //     }
                    // }
                    // else
                    // {
                        if (hist_ind > 10)
                        {
                            for (int i = hist_ind - 10; i < hist_ind; i++)
                                printf("%s\n", history_arr[i]);
                        }

                        else
                        {
                            for (int i = 0; i < hist_ind; i++)
                                    printf("%s\n", history_arr[i]);

                            // int a = history_call(ret, 10 - hist_ind);
                            // if (a == -1)
                            // {
                            //     for (int i = 0; i < hist_ind; i++)
                            //         printf("%s\n", history_arr[i]);
                            // }
                            // else
                            // {
                            //     for (int i = 0; i < hist_ind; i++)
                            //         printf("%s\n", history_arr[i]);
                            // }
                        }
                    // }
                }

                else // ye hain forward processes
                {
                    printf("Hi\n");
                    int id = fork();
                    int status;
                    if (id > 0)
                        waitpid(id, &status, WCONTINUED | WUNTRACED);

                    if (id == 0)
                    {
                        printf("here\n");
                        token[no] = NULL;
                        // char *args[] = {token[0], token[1], NULL};
                        int err = execvp(token[0], token);
                        if (err == -1)
                            printf("error: command not found\n");

                        return 0;
                    }
                }
                p_i++;
            }

        here:
            // ... printing directory for next command

            print_dir();
        }
    }
    hist_clear(history_arr, hist_ind);
}