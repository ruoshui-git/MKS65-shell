#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "parser.h"
#include "utils.h"
#include "lexer.h"

#ifndef MAX_STR_CONST
#define MAX_STR_CONST 1024
#endif

extern char *yytext;
extern char str_buf[MAX_STR_CONST];

extern char *strdup(const char *s);

int multiple_commands = 0;

struct Cmd *cmd = NULL;
struct WordList *wl = NULL;
struct WordElem *we = NULL;

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
    // keep track of lexer token
    enum TOKENS token;

    // define return option and set default values
    struct CmdOption option;
    option.status = 0;
    option.cmd = NULL;

    while ((token = yylex()))
    {
        switch (token)
        {
        case WORD:
            we = make_word(yytext);
            wl = wl_append(wl, we);
            break;

        case QUOTED_WORD:
            we = make_word(str_buf);
            wl = wl_append(wl, we);
            break;

        case RDRT_READ:

            if (strlen(yytext) != 1)
            {
                serror("does not accept descriptor for input redirect, defaulting to stdin");
            }
            token = yylex();
            char *file;
            if (token == WORD)
            {
                file = yytext;
            }
            else if (token == QUOTED_WORD)
            {
                file = str_buf;
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
                wl = NULL;
            }
            else if (wl)
            {
                cmd = cmd_append_redirect(make_cmd(wl), 0, 0, file);
                wl = NULL;
            }
            option.cmd = cmd;

            break;
        case RDRT_WRITE:
            // default action is write to stdout
            int src_fileno = 1;

            int len = strlen(yytext);
            char * rd_text = strdup(yytext);

            // if there is a descriptor, change to that one
            if (len == 2 && yytext[0] == '&')
            {
                // '&' is the only special char here
                // change of course, not supporting this as of now
            }
            if (strlen(yytext) > 1)
            {
                rd_text[strlen(rd_text) - 1] = '\0';

            }

            break;
        case RDRT_APPEND:

            break;
        case PIPE:
            break;

        case SEMICOLON:
            multiple_commands = 1;
            if (cmd)
            {
                wl = NULL;
                option.cmd = cmd;
                option.status = 1;
                return option;
            }
            else if (wl)
            {
                option.cmd = cmd = make_cmd(wl);
                wl = NULL;
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
            skip_to_end();
            // option.status = 2;
            // return option;
        }
    }

    // yylex() returned 0, reached end of line, terminate

    // Either this is a new shell command, or there are multiple commands. Then run the current one.
    // Otherwise there is no command, then return NULL
    if ((!cmd && wl) || multiple_commands)
    {
        option.cmd = cmd = make_cmd(wl);
        wl = NULL;
    }
    // else
    // {
        
    //     option.cmd = NULL;
    // }
    return option;
}

void skip_to_end(void)
{
    enum TOKENS token;
    while ((token = yylex()) != SEMICOLON && token != END)
    {
    }
}

void restart_lexer(FILE * infile)
{
    free_cmd(cmd);
    cmd = NULL;
    multiple_commands = 0;
    yyrestart(infile);
}
