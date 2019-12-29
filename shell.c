#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>

#include <fcntl.h>

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

int shell()
{
    // ln = malloc(MAX_LN_LEN * sizeof(char));

    // if (!ln)
    // {
    //     perror("shell");
    // }

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

        // if (strlen(ln) == 1)
        // {
        //     // nothing is put in, skip
        //     continue;
        // }

        // // get rid of newline
        // ln[strlen(ln) - 1] = 0;

        // argv = parse_args(ln);

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
            // handle pipes

            // keep track of the read end of the previous pipe
            int prev_out = -1;

            int pipedes[2];
            struct Cmd *cmd = option.cmd;

            // keep track of children
            struct PidList *children = NULL;
            int fchild = 1;
            int lchild = 0;

            while (cmd)
            {
                if (cmd->pipeto)
                {

                    // if there is a child to pipe to (not the last cmd in a chain of pipes)
                    // then make a new pipe
                    if (pipe(pipedes) == -1)
                    {
                        perror("pipe");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    // if this is the last command, set stdout as the write end
                    lchild = 1;
                }

                int child_pid = fork();
                if (child_pid == -1)
                {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                else if (child_pid)
                {
                    // parent will go on to fork all children

                    // add child pid to list
                    children = lappend(children, child_pid);
                }
                else
                {
                    // child handles IO and exec cmd

                    if (!fchild)
                    {
                        // all other children but the first has to reset stdin
                        if (dup2(prev_out, STDIN_FILENO) == -1)
                        {
                            perror("dup2");
                            exit(EXIT_FAILURE);
                        }
                    }
                    if (!lchild)
                    {
                        // all other children but last has to reset stdout
                        if (dup2(pipedes[PIPE_WRITE], STDOUT_FILENO) == -1)
                        {
                            perror("dup2");
                            exit(EXIT_FAILURE);
                        }
                    }
                    
                    // closing is fine here because this is in an isolated child
                    // closing here is necessary so the child being pipe to exits instad of waits for more input
                    if (!lchild) // last child doesn't create pipe
                    {
                        close(pipedes[PIPE_READ]);
                        close(pipedes[PIPE_WRITE]);
                    }
                    if (!fchild)
                    {
                        close(prev_out);
                    }

                    exec_cmd(cmd);
                }

                cmd = cmd->pipeto;

                // closing here is necessary so the child being pipe to exits instad of waits for more input
                close(pipedes[PIPE_WRITE]);
                // after first child, prev_out will not be stdin, so we need to close it
                if (!fchild)
                {
                    close(prev_out);
                }
                prev_out = pipedes[PIPE_READ];
                
                // all other children are not the first
                fchild = 0;
            }

            // done forking children
            // now wait for all of them
            pid_t child_pid;
            while ((child_pid = ldequeue(children)) != -1)
            {
                wait_for(child_pid);
            }

            free(children);
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
                return EXIT_FAILURE;
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

                /* DEBUG
                int i = 0;
                for (i = 0; argv[i] != NULL; i++)
                {
                    printf("argv[%d]: \"%s\"\n", i, argv[i]);
                }
                */

                // int rd_result = handle_redirects(option.cmd->redirects);

                // if (rd_result == -1)
                // {
                //     serror("redirection error");
                // }

                // int res = execvp(argv[0], argv);
                // // if exec succeeded, this shoudn't run
                // perror(argv[0]);
                // // puts("child: cmd not found");
                // exit(EXIT_FAILURE);

                exec_cmd(option.cmd);
            }
        }

        // clean up
        free(argv);
        argv = NULL;
    }
}