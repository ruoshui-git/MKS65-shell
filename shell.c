#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include "processes.h"
#include "parser.h"
#include "utils.h"
#include "lexer.h"
#include "cmds.h"

#include "shell.h"

extern const int MAX_LN_LEN;
// extern volatile sig_atomic_t RUNNING;
int should_restart_lexer = 0;
int should_prompt = 1;

/** Keep track of arguments */
char **argv;

/** Keep track of current line of input */
char *ln;

void shell()
{

    struct CmdOption option;

    while (1)
    {
        if (should_prompt)
        {
            prompt();
        }
        // fgets(ln, MAX_LN_LEN, stdin);

        if (should_restart_lexer)
        {
            restart_lexer(stdin);
            should_restart_lexer = 0;
        }

        // Get command from parser
        option = readCmd();

        // deal with parser output
        if (option.status == 0)
        {
            should_restart_lexer = 1;
            should_prompt = 1;
            if (!(option.cmd))
            {
                continue;
            }
        }
        else if (option.status == EOF)
        {
            shell_exit();
        }
        else if (option.status == 1)
        {

            should_prompt = 0;
        }
        else if (option.status == -1)
        {
            should_restart_lexer = 1;
            should_prompt = 1;
            free(argv);
            argv = NULL;
            continue;
        }

        argv = cmd_to_argv(option.cmd);

        // command execution
        if (strcmp(argv[0], "exit") == 0)
        {
            shell_exit();
        }
        else if (strcmp(argv[0], "cd") == 0)
        {
            cd(argv[1]);
            continue;
        }

        if (option.cmd->pipeto)
        {
            handle_pipes(option.cmd);
        }
        else
        {
            // no pipes

            // fork process to run command
            pid_t child_pid = fork();
            if (child_pid == -1)
            {
                perror("fork failure");
                cleanup();
                exit(EXIT_FAILURE);
            }
            else if (child_pid)
            {
                // I'm the parent since child id is true
                // printf("%s: command not found", ln);
                wait_for(child_pid);
            }
            else
            {
                // I'm the child since "parent" == 0
                exec_cmd(option.cmd);
            }
        }

        // clean up
        free(argv);
        argv = NULL;
    }
}