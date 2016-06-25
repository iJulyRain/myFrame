/*
 * =====================================================================================
 *
 *       Filename:  monitor.c
 *
 *    Description:  monitor
 *
 *        Version:  1.0
 *        Created:  04/01/2016 01:30:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lizhixian (group3), lizhixian@integritytech.com.cn
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
    pid_t pid = 0;
    const char *filename;

    if (argc < 2)
    {
        fprintf(stderr, "we need program filename!\n");
        return -1;
    }

    filename = argv[1];

    if (access(filename, X_OK))
    {
        fprintf(stderr, "check '%s' is exist!\n", filename);
        return -1;
    }

    while(1)
    {
        pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "fork fatal!\n");
            return -1;
        }
        else if (pid == 0) //children
        {
            fprintf(stdout, "==>start '%s'\n", filename);
            execv(filename, argv + 1);
        }

        fprintf(stdout, "==>restart '%s'\n", filename);
        wait(NULL);
    }

    return 0;
}
