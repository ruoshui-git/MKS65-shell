#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "utils.h"
#include "cmds.h"
#include "parser.h"
#include "shell.h"

extern const int MAX_LN_LEN;
extern volatile sig_atomic_t RUNNING;

/** Keep track of arguments */
char ** argv;

/** Keep track of current line of input */
char * ln;

int shell()
{
    ln = malloc(MAX_LN_LEN * sizeof(char));

    if (!ln)
    {
        perror("shell");
    }

    while (RUNNING)
    {
        prompt();
        fgets(ln, MAX_LN_LEN, stdin);

        if (strlen(ln) == 1)
        {
            // nothing is put in, skip
            continue;
        }

        // get rid of newline
        ln[strlen(ln) - 1] = 0;

        argv = parse_args(ln);

        if ('\04' == argv[0][0] || strcmp(argv[0], "exit") == 0)
        {
            puts("exit");
            cleanup();
            return EXIT_SUCCESS;
        }
        else if (strcmp(argv[0], "cd") == 0)
        {
            cd(argv[1]);
            continue;
        }

        // fork process to run command
        pid_t parent = fork();
        if (parent == -1)
        {
            perror("fork failure");
            cleanup();
            return EXIT_FAILURE;
        }
        else if (parent)
        {
            // I'm the parent since child id is true
            // printf("%s: command not found", ln);
            // do stuff
            int wstatus;
            wait(&wstatus);
            if (!WIFEXITED(wstatus))
            {
                if (WIFSIGNALED(wstatus))
                {
                    psignal(WTERMSIG(wstatus), "Exit signal: ");
                }
            }
            // printf("child status: %d\n", wstatus);
        }
        else
        {
            // I'm the child since "parent" == 0
            int i = 0;
            for (i = 0; argv[i] != NULL; i++)
            {
                printf("argv[%d]: \"%s\"\n", i, argv[i]);
            }

            int res = execvp(argv[0], argv);
            // if exec succeeded, this shoudn't run
            perror(argv[0]);
            // puts("child: cmd not found");
            return EXIT_FAILURE;
        }
        free(argv);
    }
}