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