#include <stdlib.h>

#include "parser.h"
#include "ast.h"

extern char * yytext;
extern char * str_buf;

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

struct CmdList * readCmd()
{
    enum TOKENS token;
    struct CmdList * cl = NULL;
    struct Cmd * cmd = NULL;
    struct WordList * wl = NULL;
    struct WordElem * we = NULL;

    while((token = yylex()))
    {
        switch (token)
        {
            case WORD:
                wl = wl_append(wl, make_word(yytext));
            break;

            case QUOTED_WORD:
                wl = wl_append(wl, make_word(str_buf));
            break;

            case RDRT_READ:
            break;
            case RDRT_WRITE:
            break;
            case RDRT_APPEND:
            break;
            case PIPE:
            break;

            case EOL:

            break;
            
        }
    }
}