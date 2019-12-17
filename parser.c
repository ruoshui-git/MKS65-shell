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

/** Whether there are multiple commands */
int multiple_commands = 0;

/** Whether in a pipe or not */
int in_pipe = 0;

struct Cmd * pipe_parent = NULL;

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

    while (token = yylex())
    {
        char *file;
        int status;
        int src_fileno;

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

            // does not allow redeclaration, so reset "file"
            file = NULL;

            status = lex_rd_file(&file);
            if (status == -1)
            {
                continue;
            }

            if (!cmd && wl)
            {
                cmd = make_cmd(wl);
            }
            cmd = cmd_append_redirect(cmd, 0, 0, file);
            wl = NULL;
            option.cmd = cmd;

            break;

        case RDRT_WRITE:
        case RDRT_APPEND: //largely the same

            ; // empty statement to avoid error "a label can only be part of a statement and a declaration is not a statement"

            // default action is redirect stdout
            src_fileno = 1;

            int len = strlen(yytext);
            char *rd_text = strdup(yytext);

            // does not allow redeclaration so reset "file"
            file = NULL;
            status = lex_rd_file(&file);
            if (status == -1)
            {
                continue;
            }

            // deal with descriptors and add redirect to cmd
            if (rd_text[0] == '&')
            {
                if (token == RDRT_APPEND)
                {
                    serror("shell does not support [num]>>&[num]");
                    skip_to_end();
                    continue;
                }

                // '&' is the only special char here
                // it would get converted to 1>[file] 2>&1
                if (!cmd && wl)
                {
                    cmd = make_cmd(wl);
                }
                wl = NULL;
                cmd = cmd_append_redirect(cmd, 1, 1, file);
                cmd = cmd_append_redirect(cmd, 1, 2, strdup("&1"));

                free(rd_text);
                break;
            }
            else if ((token == RDRT_WRITE && len > 1) || (token == RDRT_APPEND && len > 2))
            {
                // this means "[num]>"
                src_fileno = get_rd_fileno(rd_text);
            }
            // else
            // {
            //     // ">"
            //     src_fileno = 1; // 1 == stdout
            // }

            // no longer needed
            free(rd_text);

            // append redirect
            if (!cmd && wl)
            {
                cmd = make_cmd(wl);
            }

            wl = NULL;
            cmd_append_redirect(cmd, (token == RDRT_WRITE ? 1 : 2), src_fileno, file);

            option.cmd = cmd;

            break;

        case PIPE:
            // if already in a pipe, start new pipe
            if (in_pipe)
            {
                if (cmd)
                {
                    pipe_parent = attach_pipe(pipe_parent, cmd);
                }
            }
            else
            {
                in_pipe = 1;
                if (cmd)
                {
                    pipe_parent = cmd;
                    cmd = NULL;
                }
                else if (wl)
                {
                    pipe_parent = make_cmd(wl);
                    cmd = NULL;
                }
                else
                {
                    pserror("needs a command for pipe");
                    skip_to_end();
                    return -1;
                }
            }
            
            if (!cmd && wl)
            {
                cmd = make_cmd(wl);
            }
            
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
    // Otherwise there is no command, then return NULL, which is the default
    if ((!cmd && wl) || multiple_commands)
    {
        option.cmd = cmd = make_cmd(wl);
        wl = NULL;
    }
    return option;
}

void skip_to_end(void)
{
    enum TOKENS token;
    while ((token = yylex()) != SEMICOLON && token != END)
    {
    }
}

void restart_lexer(FILE *infile)
{
    if (cmd)
    {
        free_cmd(cmd);
    }
    cmd = NULL;
    multiple_commands = 0;
    yyrestart(infile);
}

int get_rd_fileno(char *rd_text)
{
    char *text = strdup(rd_text);
    int len = strlen(text);
    if (text[len - 2] == '>')
    {
        // then it's a "[num]>>"
        text[len - 2] = '\0';
    }
    else
    {
        // "[num]>"
        text[len - 1] = '\0';
    }
    

    int fd = atoi(text);
    free(text);
    return fd;
}

int lex_rd_file(char **file_ptr)
{
    enum TOKENS token = yylex();
    if (token == WORD)
    {
        *file_ptr = yytext;
    }
    else if (token == QUOTED_WORD)
    {
        *file_ptr = str_buf;
    }
    else
    {
        pserror("expects a file after redirect");
        // skip the section affected by error
        skip_to_end();
        return -1;
    }
    return 0;
}
