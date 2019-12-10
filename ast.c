#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "utils.h"

struct WordList *wl_append(struct WordList *wl, struct WordElem *e)
{
    if (!wl)
    {
        wl = calloc(sizeof(struct WordList), 1);
    }
    
    if (!wl->head)
    {
        wl->head = e;
        wl->end = e;
        wl->len = 1;
    }
    else
    {
        wl->end->next = e;
        e->prev = wl->end;
        wl->len++;
    }

    return wl;
}

struct WordElem *make_word(char *str)
{
    struct WordElem *we = malloc(sizeof(struct WordElem));
    we->word = malloc(strlen(str)); // or replace with strdup, should be the same thing
    strcpy(we->word, str);
    we->next = NULL;
    we->prev = NULL;
    return we;
}

struct Cmd *make_cmd(struct WordList *wl)
{
    struct Cmd *cmd = malloc(sizeof(struct Cmd));
    cmd->cmd = wl;
}

struct Cmd *cmd_append_redirect(struct Cmd *cmd, int rdrt_type, int src_fileno, char *file)
{
    struct Redirect *rd = cmd->redirects;
    struct Redirect * new_rd = malloc(sizeof(struct Redirect));
    new_rd->filename = strdup(file);
    new_rd->next = NULL;
    new_rd->type = rdrt_type;
    new_rd->src_fileno = src_fileno;

    if (rd)
    {
        while (rd->next)
        {
            rd = rd->next;
        }
        rd->next = new_rd;
    }
    else
    {
        cmd->redirects = new_rd;
    }
    return cmd;
}

char ** cmd_to_argv(struct Cmd * cmd)
{
    struct WordList * wl = cmd->cmd;
    int len = wl->len;
    char ** argv = malloc(sizeof(char*) * len);
    int i;
    struct WordElem * we;
    for (i = 0, we = wl->head; i < len; i++, we = we->next)
    {
        if (!we) 
        {
            iserror("argument conversion error");
        }
        argv[i] = we->word;
    }
    return argv;
}