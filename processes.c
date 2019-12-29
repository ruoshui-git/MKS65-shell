#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// for wait and receive signals from children
#include <sys/wait.h>

#include "cmds.h"
#include "ast.h"
#include "utils.h"
#include "processes.h"

int handle_redirects(struct Redirect *rd)
{
    if (!rd)
    {
        return -2;
    }
    while (rd)
    {
        int type = rd->type;
        if (type == 0)
        {
            // < redirect
            int fd = open(rd->filename, O_RDONLY | __O_CLOEXEC);
            if (fd == -1)
            {
                perror("open");
                return -1;
            }
            if (dup2(fd, STDIN_FILENO) == -1)
            {
                perror("dup2");
            }
        }
        else
        {
            // before, type of redirect separates ctl flow; now we separate based on file starts with "&" or not
            // a file that starts with "&" mark a fileno, not an actual file
            if (rd->filename[0] == '&')
            {
                // convert the rest to a file descriptor
                int dest_fileno = atoi(&(rd->filename[1]));
                if (dup2(dest_fileno, rd->src_fileno) == -1)
                {
                    perror("dup2");
                    return -1;
                }
            }
            else
            {

                int flag;
                if (type == 1)
                {
                    flag = O_TRUNC;
                }
                else if (type == 2)
                {
                    // type == 2
                    flag = O_APPEND;
                }

                int fd = open(rd->filename, O_WRONLY | O_CREAT | __O_CLOEXEC | flag, 0666);
                if (fd == -1)
                {
                    perror("open");
                    return -1;
                }
                int d_result = dup2(fd, rd->src_fileno);
                if (d_result == -1)
                {
                    perror("dup2");
                    return -1;
                }
            }
        }
        rd = rd->next;
    }
    return 0;
}

void exec_cmd(struct Cmd *cmd)
{
    fprintf(stderr, "exec_cmd: %s\n", cmd->cmd->head->word);
    if (cmd->redirects)
    {
        int rd_result = handle_redirects(cmd->redirects);

        if (rd_result == -1)
        {
            serror("redirection error");
        }
    }

    // if (cmd->pipeto)
    // {
    //     int pipedes[2];
    //     if (pipe(pipedes) == -1)
    //     {
    //         perror("pipe");
    //     }
    //     if (dup2(pipedes[PIPE_WRITE], STDOUT_FILENO) == -1)
    //     {
    //         perror("dup2");
    //     }
    //     int parent = fork();
    //     if (parent == -1)
    //     {
    //         perror("fork");
    //     }
    //     else if (parent)
    //     {
    //         // do nothing and move on
    //     }
    //     else
    //     {
    //         if (dup2(pipedes[PIPE_READ], STDIN_FILENO) == -1)
    //         {
    //             perror("dup2 in child process");
    //         }
    //         // execute the command to be piped to
    //         exec_cmd(cmd->pipeto);
    //     }
    // }

    char **argv = cmd_to_argv(cmd);

    int res = execvp(argv[0], argv);
    // if exec succeeded, this shoudn't run
    perror(argv[0]);
    // puts("child: cmd not found");
    exit(EXIT_FAILURE);
}

void wait_for(pid_t cpid)
{

    int wstatus;

    waitpid(cpid, &wstatus, 0);
    if (!WIFEXITED(wstatus))
    {
        if (WIFSIGNALED(wstatus))
        {
            psignal(WTERMSIG(wstatus), "Exit signal: ");
        }
    }
    // printf("child status: %d\n", wstatus);
}

void handle_pipes(struct Cmd *cmd)
{

    // handle pipes

    // keep track of the read end of the previous pipe
    int prev_out = -1;

    int pipedes[2];

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

int get_std_fileno(int src_fileno)
{
    switch (src_fileno)
    {
    case 0:
        return STDIN_FILENO;
        break;
    case 1:
        return STDOUT_FILENO;
        break;
    case 2:
        return STDERR_FILENO;
        break;
    default:
        // unrecognized
        return -1;
        break;
    }
}