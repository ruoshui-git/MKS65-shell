#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "parser.h"
#include "utils.h"

extern char * yytext;
extern char * str_buf;
#include "lexer.h"

extern char *strdup(const char *s);

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

struct CmdOption readCmd()
{
    enum TOKENS token;
    
    struct CmdOption option;

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

                if (strlen(yytext) != 1)
                {
                    serror("does not accept descriptor for input redirect, defaulting to stdin");
                }
                token = yylex();
                char * file;
                if (token == WORD || token == QUOTED_WORD)
                {
                    file = yytext;
                }
                else
                {
                    pserror("expects a file after redirect");
                    option.status = 2;
                    // skip the section affected by error
                    skip_to_end();
                    // return option;
                }
                if (cmd)
                {
                    cmd = cmd_append_redirect(cmd, 0, 0, file);
                }
                else if (wl)
                {
                    cmd = cmd_append_redirect(make_cmd(wl), 0, 0, file);
                    wl = NULL;
                    we = NULL;
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
                    wl = NULL;
                    we = NULL;
                    option.cmd = cmd;
                    option.status = 1;
                    return option;
                }
                else if (wl)
                {
                    struct Cmd * cmd = make_cmd(wl);
                    wl = NULL;
                    we = NULL;
                    option.cmd = cmd;
                    option.status = 1;
                    return option;
                }
                else
                {
                    pserror("expects a command");
                    option.status = -1;
                    return option;
                }
            break;
            default:
                pserror("unrecognized token");
                // don't stop, continue parsing!
                // option.status = 2;
                // return option;
        }

        // yylex() returned 0, reached end of line, terminate
        option.status = 0;
        return option;
    }
}

void skip_to_end(void)
{
    enum TOKENS token;
    while ((token = yylex()) != SEMICOLON || token != END) {}
}