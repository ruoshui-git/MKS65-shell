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

struct Cmd * clear_cmd(struct Cmd * cmd)
{
    if (!cmd)
    {
        return NULL;
    }
    cmd->cmd = free_word_list(cmd->cmd);
    cmd->redirects = free_redirects(cmd->redirects);
    cmd->pipeto = free_cmd(cmd->pipeto);
    return cmd;
}

struct Cmd * free_cmd(struct Cmd * cmd)
{
    if (cmd)
    {
        cmd = clear_cmd(cmd);
        free(cmd);
    }
    return NULL;
}

struct WordList * free_word_list(struct WordList * wl)
{
    if (wl)
    {
        // free the entire list in a loop
        struct WordElem * cur, * next;
        cur = wl->head;
        next = wl->head;
        while (next = next->next)
        {
            free(cur->word);
            free(cur);
            cur = next;
        }
        free(cur->word);
        free(cur);

        // free list
        free(wl);
    }
    return NULL;
}

struct Redirect * free_redirects(struct Redirect * rd)
{
    if (rd)
    {
        struct Redirect * cur, * next;
        cur = rd;
        next = rd;
        while (next = rd->next)
        {
            free(cur);
            cur = next;
        }
        free(cur);
    }
    return NULL;
}
