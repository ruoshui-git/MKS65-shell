#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "colors.h"

#define PATH_MAX 4096

char cwd[PATH_MAX];

void cleanup()
{
    // puts("cleaning up...");
    puts("exit");
    free(argv);
    free(ln);
}

int countchar(char * src, char c)
{
    int count, i;
    int len = strlen(src);
    for (i = 0, count = 0; i < len; i++)
    {
        if (src[i] == c)
        {
            count++;
        }
    }
    return count;
}


void prompt()
{
    getcwd(cwd, PATH_MAX);
    printf(BOLD_BLUE UNDERLINE"%s"RESET"# ", cwd);
    // fputs("Prompt # ", stdout);
}