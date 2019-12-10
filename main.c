#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>

#include "shell.h"
#include "utils.h"


/** max command length */
const int MAX_LN_LEN = 1000;

volatile sig_atomic_t RUNNING = 1;

/**
 * Handle all the signals
 */
static void sighandler(int signum)
{
    if (signum == SIGINT)
    {
        printf("\n");
        prompt();
        return;
    }
    else if (signum == SIGSEGV)
    {
        cleanup();
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    signal(SIGINT, sighandler);
    signal(SIGSEGV, sighandler);
    return shell();
}
