#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"

struct WordList *wl_append(struct WordList *wl, struct WordElem *e)
{
    if (!wl)
    {
        wl = calloc(sizeof(struct WordList), 1);
    }
    else if (!wl->head)
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

struct WordElem * make_word(char * str)
{
    struct WordElem * we = malloc(sizeof(struct WordElem));
    we->word = malloc(strlen(str)); // or replace with strdup, should be the same thing
    strcpy(we->word, str);
    we->next = we->prev = NULL;
    return we;
}