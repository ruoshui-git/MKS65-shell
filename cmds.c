#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cmds.h"
#include "utils.h"

void cd(const char * path)
{
    int stat;
    if (path)
    {
        stat = chdir(path);
    }
    else
    {
        puts("cd .");
        stat = chdir(".");
    }
    
    if (stat != 0)
    {
        perror("cd");
        printf("address: %s\n", path);
    }
}

void shell_exit(void)
{
    
    puts("exit");
    cleanup();
    exit(EXIT_SUCCESS);
}