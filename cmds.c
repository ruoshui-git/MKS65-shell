#include <stdio.h>
#include <unistd.h>

#include "cmds.h"

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