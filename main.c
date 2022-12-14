#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
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

typedef struct bg_proc_
{
    char name[100];
    int run;
    int index;
    int pid;
} bg_proc;

int len;
char *user;
char sysname[100];
int ulen;
bg_proc bg_starr[1000];
int bg_i;
int x;
int fd;
int home_fd;

int compare(const void *p1, const void *q1)
{
    return strcasecmp(*(const char **)p1, *(const char **)q1);
}
void swap_func(int i, int j)
{
    bg_proc x;
    memcpy(&x, &bg_starr[i], sizeof(bg_proc));
    memcpy(&bg_starr[i], &bg_starr[j], sizeof(bg_proc));
    memcpy(&bg_starr[j], &x, sizeof(bg_proc));
}
void handle_sigquit(int signum);
void handle_sigtstp(int signum) // ctrl + Z
{
    return;
}
void signal_handler(int signum)
{
    for (int i = 0; i < bg_i; i++)
    {
        int status;
        if (bg_starr[i].pid != 0 && waitpid(bg_starr[i].pid, &status, WNOHANG) != 0)
        {
            if (WIFEXITED(status))
                printf("pid %d exited normally\n", bg_starr[i].pid);

            else
                printf("pid %d exited abnormally\n", bg_starr[i].pid);

            bg_starr[i].pid = 0;
            bg_starr[i].index = 0;
            // for( int j = i; j< bg_i-1; j++ )
            // {
            //     bg_arr[j] = bg_arr[j+1];
            //     bg_starr[j+1].index--;
            //     // swap_func(j, j+1);
            // }

            // i--;
            // bg_i--;
            // printf("changing this : %s\n", bg_starr[i].name);
            strcpy(bg_starr[i].name, " ");
            if (i == bg_i - 1)
                bg_i--;

            else
                bg_i++;
            // i--;
        }
    }
}
void handle_sigint(int signum) // ctrl +
{
    printf("\n Exiting Terminal...\n");
    exit(EXIT_SUCCESS);
}
int main()
{
    // printf("\033[;30m\n\n\n\t\t**WELCOME TO MY SHELL**\n\n\n\033[0m");
    char *history_arr[1000];
    int hist_ind = 0;
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
    bg_i = 0;
    signal(SIGCHLD, signal_handler);
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);
    // signal(SIGQUIT, handle_sigquit);
    while (1)
    {
        char input[100];
        if (fgets(input, 100, stdin) == NULL)
        {
            printf("\n\nLogging out of Shell...\n");
            goto quiting;
        }

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
                not__ = 1; // so there's no & symbol for last command
            }

            if (hist_ind > 0 && strcmp(commands[c], history_arr[hist_ind - 1]) != 0)
            {
                history_arr[hist_ind] = (char *)malloc(sizeof(char *) + 1);
                strcpy(history_arr[hist_ind], commands[c]);
                hist_ind++;
            }
            else if (hist_ind == 0)
            {
                history_arr[0] = (char *)malloc(sizeof(char) + 1);
                strcpy(history_arr[0], commands[c]);
                hist_ind++;
            }

            // .....
            parts[0] = strtok(commands[c], "&");

            int p_no = 1, p_i = 0;
            while (parts[p_no - 1] != NULL)
            {
                parts[p_no] = strtok(NULL, "&");

                p_no++;
            }
            p_no--;
            while (p_i < p_no)
            {
                int out_fp, pipe_no = 1;
                char *piped[10];
                piped[0] = strtok(parts[p_i], "|");
                while (piped[pipe_no - 1] != NULL)
                {
                    piped[pipe_no] = strtok(NULL, "|");
                    pipe_no++;
                }
                pipe_no--;

                home_fd = dup(1);
                int in_fd = dup(0);

                int p_arr[2];

                for (int pip_i = 0; pip_i < pipe_no; pip_i++) // for every piped[pip_i]
                {
                    // printf("bg_i : %d\n", bg_i);
                    // printf("%d\n", pip_i);
                    pipe(p_arr);
                    dup2(p_arr[1], 1);
                    if (pip_i == pipe_no - 1)
                    {
                        close(p_arr[1]);
                        dup2(home_fd, 1);
                    }

                    for (int h = 0; h < strlen(piped[pip_i]); h++)
                    {
                        char *this;
                        if (piped[pip_i][h] == '>')
                        {
                            if (piped[pip_i][h + 1] == '>')
                            {
                                this = strtok(&piped[pip_i][h + 2], " ");
                                out_fp = open(this, O_CREAT | O_WRONLY | O_APPEND, 0644);
                                piped[pip_i][h + 2] = '\0';
                            }

                            else
                            {
                                this = strtok(&piped[pip_i][h + 1], " ");
                                out_fp = open(this, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                                piped[pip_i][h + 1] = '\0';
                            }
                            fd = dup(1);
                            dup2(out_fp, 1);
                            break;
                        }

                        else if (piped[pip_i][h] == '<')
                        {
                        }
                    }

                    char *token[10];
                    token[0] = strtok(piped[pip_i], " \t\n><");
                    if (token[0] == NULL)
                        continue;
                    if (strcmp(token[0], "q") == 0)
                    {
                    quiting:
                        hist_clear(history_arr, hist_ind);
                        return 0;
                    }

                    int no = 1;
                    while (token[no - 1] != NULL)
                    {
                        token[no] = strtok(NULL, " \t\n><");
                        no++;
                    }
                    no--;

                    if ((number == 1) || (number == 0 && p_i != p_no - 1 && p_no > 1))
                    {
                        int id = fork();
                        if (id > 0)
                        {
                            int max_ind = 0;
                            for (int k = 0; k < 100; k++)
                            {
                                if (bg_starr[k].pid != 0)
                                {
                                    if (bg_starr[k].index > max_ind)
                                        max_ind = bg_starr[k].index;
                                }
                            }
                            if (max_ind != 0)
                                bg_i = max_ind + 1;
                            printf("[%d] %d\n", bg_i + 1, id);
                            bg_starr[bg_i].index = bg_i;
                            strcpy(bg_starr[bg_i].name, piped[pip_i]);
                            if (tcgetpgrp(STDIN_FILENO) < 0)
                                bg_starr[bg_i].run = 0;

                            else
                                bg_starr[bg_i].run = 1;

                            bg_starr[bg_i].pid = id;
                            bg_i++;
                        }

                        if (id == 0)
                        {
                            token[no] = NULL;

                            int err = execvp(token[0], token);
                            if (err == -1)
                                printf("error : command not found\n");

                            return 0;
                        }
                    }

                    else if (strcmp("echo", token[0]) == 0)
                    {
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
                            if (dd == NULL)
                                printf("error: no such directory\n");

                            struct dirent *str_dir;
                            str_dir = (struct dirent *)malloc(sizeof(struct dirent *) + 1);
                            str_dir = readdir(dd);
                            int index2 = 0;
                            while (str_dir != NULL)
                            {
                                files1[index2] = (char *)malloc(1000 * sizeof(char *));
                                strcpy(files1[index2], str_dir->d_name);
                                index2++;
                                str_dir = readdir(dd);
                            }
                            closedir(dd);
                            qsort(files1, index2, sizeof(char *), compare);

                            if (flagl == 1)
                                ls_l(files1, index2, path, flaga);

                            else
                                ls_only(files1, index2, path, flaga);

                            // goto here;
                            if (pip_i == pipe_no)
                                print_dir();

                            dup2(p_arr[0], 0);
                            continue;
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

                        int index2;
                        for (int i = 0; i < f_index; i++)
                        {
                            char path0[500];
                            getcwd(path0, 500);
                            cdir(folders[i], home_dir, 1);
                            char *files[500];
                            char path[500];
                            getcwd(path, 500);
                            DIR *dd = opendir(path);
                            struct dirent *str_dir;
                            str_dir = (struct dirent *)malloc(sizeof(struct dirent));
                            str_dir = readdir(dd);
                            index2 = 0;
                            while (str_dir != NULL)
                            {
                                files[index2] = (char *)malloc(strlen(str_dir->d_name) * sizeof(char));
                                strcpy(files[index2], str_dir->d_name);
                                index2++;
                                str_dir = readdir(dd);
                            }
                            qsort(files, index2, sizeof(char *), compare);

                            if (flagl == 1)
                                ls_l(files, index2, path, flaga);

                            else
                                ls_only(files, index2, path, flaga);

                            cdir(path0, home_dir, 1);
                            closedir(dd);
                        }
                        if (f_index == 0 && file_index == 0)
                            goto back;

                        for (int i = 0; i < index2; i++)
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

                                if (strcmp(store[t], "..") == 0)
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
                        if (hist_ind > 10)
                        {
                            for (int i = hist_ind - 10; i < hist_ind; i++)
                                printf("%s\n", history_arr[i]);
                        }

                        else
                        {
                            for (int i = 0; i < hist_ind; i++)
                                printf("%s\n", history_arr[i]);
                        }
                    }

                    // else if (strcmp(token[0], "sig") == 0)
                    // {
                    //     int job_no = atoi(token[1]);
                    //     int sig_no = atoi(token[2]);
                    //     signal_call(job_no, sig_no);
                    // }

                    else if (strcmp(token[0], "bg") == 0)
                    {
                        if (token[1] == NULL)
                        {
                            printf("error: missing operands\n");
                            return 0;
                        }
                        int bgno = atoi(token[1]);
                        if (bg_i < bgno)
                            printf("error: background process doesnot exist");

                        kill(bg_starr[bgno].pid, SIGCONT);
                        // bg_arr[bg_i] = 0;
                    }

                    else if (strcmp(token[0], "jobs") == 0)
                    {
                        int r_flag = 0, s_flag = 0;
                        if (token[1] == NULL)
                        {
                            r_flag = 1;
                            s_flag = 1;
                        }
                        else if (strcmp(token[1], "-r") == 0)
                            r_flag = 1;

                        else if (strcmp(token[1], "-s") == 0)
                            s_flag = 1;

                        // printf("bg_i : %d\n", bg_i);
                        for (int i = 0; i < bg_i; i++)
                        {
                            for (int j = i + 1; j < bg_i; j++)
                            {
                                // if( strcmp(bg_starr[j].name, " ") == 0)
                                //     continue;
                                if (strcmp(bg_starr[i].name, bg_starr[j].name) > 0)
                                {
                                    swap_func(i, j);
                                }
                            }
                        }
                        for (int i = 0; i < bg_i; i++)
                        {
                            if (strcmp(bg_starr[i].name, " ") == 0)
                                continue;

                            printf("[%d] ", bg_starr[i].index + 1);
                            if (bg_starr[i].run = 1 && r_flag == 1)
                                printf("Running ");

                            else if (s_flag == 1)
                                printf("Stopped ");

                            else
                                continue;
                            printf("%s [%d]\n", bg_starr[i].name, bg_starr[i].pid);
                        }
                    }

                    else if (strcmp(token[0], "fg") == 0)
                    {
                        if (token[1] == NULL)
                        {
                            printf("error: missing operand\n");
                            return 0;
                        }
                        int status;
                        int fgno = atoi(token[1]);
                        if (bg_i < fgno)
                            printf("error: background process doesnot exist\n");

                        kill(bg_starr[fgno].pid, SIGCONT);
                        bg_starr[fgno].pid = 0;
                        waitpid(bg_starr[fgno].pid, &status, WUNTRACED);
                    }

                    else if (strcmp(token[0], "sig") == 0)
                    {
                        int a1 = atoi(token[1]);
                        int a2 = atoi(token[2]);
                        if (bg_i < a1)
                            printf("error: process doesnot exist");

                        for (int i = 0; i < 1000; i++)
                        {
                            if (bg_starr[i].pid != 0)
                            {
                                if (bg_starr[i].index == a1 - 1)
                                {
                                    kill(bg_starr[i].pid, a2);
                                    break;
                                }
                            }
                        }
                    }

                    else // ye hain foreground processes
                    {
                        int pid_ = getpid();
                        int id = fork();
                        int status;
                        if (id > 0)
                        {
                            waitpid(id, &status, WCONTINUED | WUNTRACED);
                            if (WIFSTOPPED(status))
                            {
                                int max_ind = 0;
                                for (int k = 0; k < 100; k++)
                                {
                                    if (bg_starr[k].pid != 0)
                                    {
                                        if (bg_starr[k].index > max_ind)
                                            max_ind = bg_starr[k].index;
                                    }
                                }
                                if (max_ind != 0)
                                    bg_i = max_ind + 1;
                                // printf("hiiii\n");
                                bg_starr[bg_i].index = bg_i;
                                strcpy(bg_starr[bg_i].name, piped[pip_i]);
                                char buffer[100];
                                int a;
                                char timepass[1000];
                                sprintf(buffer, "/proc/%d/stat", id);
                                FILE *fd = fopen(buffer, "r");
                                fscanf(fd, "%d %s %c", &a, timepass, &status);
                                if( status == 'R' )
                                    bg_starr[bg_i].run = 1;

                                else if( status == 'T' )
                                    bg_starr[bg_i].run = 0;    

                                bg_starr[bg_i].pid = pid_;
                                bg_i++;
                            }
                        }
                        if (id == 0)
                        {
                            token[no] = NULL;
                            int err = execvp(token[0], token);
                            if (err == -1)
                                printf("error: command not found\n");

                            return 0;
                        }
                        // printf(" : forward process done : \n");
                    }
                    dup2(p_arr[0], 0);

                    if (pip_i != pipe_no - 1)
                        close(p_arr[1]);
                }
                dup2(in_fd, 0);
                dup2(home_fd, 1);
                p_i++;
            }
        here:
            print_dir();
        }
    }
    hist_clear(history_arr, hist_ind);
}
