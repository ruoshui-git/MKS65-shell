#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

const int ln_len = 1000;
int RUNNING = 1;

void prompt();
char ** parse_args( char * line );
int countchar(char * src, char c);

int main(void)
{
    char * ln = malloc(ln_len * sizeof(char));
    while (RUNNING)
    {
        prompt();
        fgets(ln, ln_len, stdin);

        char ** argv = parse_args(ln);

        // fork process to run command
        pid_t parent = fork();
        if (parent)
        {
            // I'm the parent since child id is true
            // printf("%s: command not found", ln);
            // do stuff
            int wstatus;
            wait(&wstatus);
            printf("child status: %d\n", wstatus);

        }
        else
        {
            // I'm the child since "parent" == 0
            int i = 0;
            for (i = 0; argv[i] != NULL; i++)
            {
                printf("argv[%d]: %s\n", i, argv[i]);
            }

            execvp(argv[0], argv);
            // puts("child: cmd not found");
            exit(1);
        }
    }
    // parse(ln, parsed_ln);
    // exec(parsed_ln);
}

char ** parsecmd(char * s)
{

}

/** free return val! */
char ** parse_args( char * line )
{
    int num_args = countchar(line, ' ');
    char ** args = malloc(sizeof(char *) * (num_args + 1));

    int i = 0;
    char * curr = line;
    char * token;
    while (curr)
    {
        args[i] = strsep(&curr, " ");
        i++;
    }
    args[i] = NULL;
    return args;
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
    fputs("Prompt # ", stdout);
}
// void parse();
// void exec();
