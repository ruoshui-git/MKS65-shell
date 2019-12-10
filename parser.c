#include <stdlib.h>

#include "parser.h"
#include "ast.h"

extern char * yytext;
extern char * str_buf;


struct Cmd * cmd = NULL;
struct WordList * wl = NULL;
struct WordElem * we = NULL;



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

struct Cmd * readCmd()
{
    enum TOKENS token;

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
                if (cmd)
                {
                    cmd = cmd_append_redirect();
                }
            break;
            case RDRT_WRITE:
            break;
            case RDRT_APPEND:
            break;
            case PIPE:
            break;

            case SEMICOLON:
                if (cmd)
                {
                    wl = we = NULL;
                    return cmd;
                }
                else if (wl)
                {
                    struct Cmd * cmd = make_cmd(wl);
                    wl = we = NULL;
                    return cmd;
                }
            break;
        }
    }
}

void pserror(char * msg)
{
    fprintf(stderr, "Parser error: %s\n", msg);
}
