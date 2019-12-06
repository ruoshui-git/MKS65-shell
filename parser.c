#include <stdlib.h>

#include "parser.h"

char **parse_args(char *line)
{
    // num of separators == num of args; + 1 to take into acount the newline
    int num_args = countchar(line, ' ') + 1;

    // + 1 here for the NULL in the end
    char **args = malloc(sizeof(char *) * (num_args + 1));

    int i = 0;
    char *curr = line;
    char *token;
    while (curr)
    {
        args[i] = strsep(&curr, " ");
        i++;
    }
    args[i] = 0;
    return args;
}
