#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>

#include "shell.h"
#include "utils.h"
#include "parser.h"

/** max command length */
const int MAX_LN_LEN = 1000;

// volatile sig_atomic_t RUNNING = 1;

/**
 * Handle all the signals
 */
static void sighandler(int signum)
{
    if (signum == SIGINT)
    {
        // printf("\n");
        // restart_lexer(stdin);
        // prompt();
        return;
    }
    else if (signum == SIGSEGV)
    {
        cleanup();
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
    signal(SIGINT, sighandler);
    signal(SIGSEGV, sighandler);

    // input stream
    FILE * infile = NULL;

    if (argc == 1)
    {
        // default, no argument
        infile = stdin;
    }
    else if (argc == 2)
    {
        // second arg is input stream
        infile = fopen(argv[1], "r");
        if (!infile)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        puts("usage:\n\t./main [file]");
        return EXIT_FAILURE;
    }
    
    if (infile)
    {
        shell(infile);
    }

    return 1;
}
