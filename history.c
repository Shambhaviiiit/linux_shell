#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "history.h"

// when program starts, open file (if exists already) and read its contents into an array
// keep adding history(each command) to this array
// when exiting program, rewrite the file :
// let x = no. of elements in array
// if ( x>=20 )
// then add last 20 elements of array to the file
// else
// rewrite the file with the array

int history_read(char *arr[1000])
{
    int i=0;
    FILE *fp = fopen("history.txt", "r");
    if( fp==NULL )
    {
        return i;
    }
    else
    {
        while(1)
        {
            arr[i] = (char*)malloc(100*sizeof(char*));
            if( fgets(arr[i], 100, fp) == NULL )
                break;

            if( arr[i][strlen(arr[i])-1] == '\n' )
                arr[i][strlen(arr[i])-1] = '\0';

            i++;    
        }        
        fclose(fp);
        return i;
    }
}
// int history_call(char **ret, int want)
// {
//     int i=0;
//     FILE *fd = fopen("history.txt", "r");
//     if( fd == NULL )
//     {
//         // printf("error : file does not exist\n");
//         // return;
//         return -1;
//     }
//     while(1)
//     {
//         ret[i] = (char*)malloc(100*sizeof(char *));
        
//         if( fgets(ret[i], 100, fd) == NULL )
//             break;
//         // if( fscanf(fd, "%s", ret[i]) == EOF )
//         //     break;

//         // printf("%s", ret[i]);
//         i++;
//     }
//     if( i>want )
//     {
//         for( int j=i-want; j<i; j++ )
//             printf("%s", ret[j]);
//     }
//     else
//     {
//         for( int j=0; j<i; j++ )
//             printf("%s", ret[j]);
//     }
//     for( int j=0; j<i; j++ )
//         free(ret[j]);

//     return i;
// }

void hist_clear(char *array[100], int total)
{
    if (total >= 20)
    {
        FILE *fd = fopen("history.txt", "w");
        // for (int i = 0; i < total; i++)
        // {
        //     fprintf(fd, "%s\n", array[i]);
        // }
        for( int i=total-20; i<total; i++ )
        {
            fprintf(fd, "%s\n", array[i]);
        }
        fclose(fd);
    }
    else
    {
        FILE *fd = fopen("history.txt", "a");
        // fseek(fd, 0, SEEK_END);

        if( fd == NULL )
        {
            printf("error: file not created\n");
            return;
        }
        for (int i = 0; i < total; i++)
        {
            fprintf(fd, "%s\n", array[i]);
            // array[i] = NULL;
        }
        fclose(fd);
    }
    return;
}

// void hist_clear(char *array[100], int till)
// {
//     if (till >= 20)
//     {
//         FILE *fd = fopen("history.txt", "w");
//         for (int i = 0; i < till; i++)
//         {
//             fprintf(fd, "%s\n", array[i]);
//             // array[i] = NULL;
//         }
//     }
//     else
//     {
//         FILE *fd = fopen("history.txt", "a");
//         // fseek(fd, 0, SEEK_END);

//         if( fd == NULL )
//         {
//             printf("error: file not created\n");
//             return;
//         }
//         for (int i = 0; i < till; i++)
//         {
//             fprintf(fd, "%s\n", array[i]);
//             array[i] = NULL;
//         }
//     }
//     return;
// }

// agar number = 1 hai toh last wala parts[] bg hai
// agar bg = 1 hai matlab parts[] has either
// only one command which is bg 
// or
// multiple commands which are all bg ( except last one, which may or may not be bg depending on number )