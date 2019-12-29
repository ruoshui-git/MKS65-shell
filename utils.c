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
    fflush(stdout);
    // fputs("Prompt # ", stdout);
}

void serror(char * msg)
{
    fprintf(stderr, "Warning: %s\n", msg);
}

void pserror(char * msg)
{
    fprintf(stderr, "Parser error: %s\n", msg);
}

void iserror(char * msg)
{
    fprintf(stderr, "Internal shell error: %s\n", msg);
}


struct PidList * lappend (struct PidList * l, int data)
{
    
    struct PidNode * node = malloc(sizeof(struct PidNode));
    node->data = data;
    node->next = NULL;
    if (!l)
    {
        l = malloc(sizeof(struct PidList));
        l->head = node;
        l->end = node;
        l->len = 1;
    }
    else
    {
        l->end->next = node;
        l->end = node;
        l->len++;
    }
    return l;
}

int ldequeue (struct PidList * list)
{
    if (!list || list->len < 1)
    {
        return -1;
    }

    struct PidNode * head = list->head;
    int data = head->data;
    list->head = head->next;

    free(head);
    list->len--;
    return data;
}