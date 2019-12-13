#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "cmds.h"
#include "ast.h"

int stdin_copy = 0;
int stdout_copy = 0;
int stderr_copy = 0;
int stdin_change = 0;
int stdout_change = 0;
int stderr_change = 0;

/**
 * Handles redirect by setting system fd to the files
 * @return 0 on success, -1 on sys error, -2 on fn error
 */
int handle_redirects(struct Cmd *cmd)
{
    if (!cmd)
    {
        return -2;
    }

    struct Redirect *rd = cmd->redirects;
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
            int fd = open(rd->filename, O_RDONLY);
            if (fd == -1)
            {
                perror("open");
                return -1;
            }
            dup2(STDIN_FILENO, fd);
        }
        else if (type == 1)
        {
            int fd = open(rd->filename, O_WRONLY);
            if (fd == -1)
            {
                perror("open");
                return -1;
            }
            dup2(STDOUT_FILENO, fd);
        }
        else if (type == 2)
        {
            int fd = open()
        }
    }
}

/**
 * Reset redirects and closed the file
 */
void reset_redirects(struct Cmd *cmd)
{
    if (stdin_change)
    {
        dup2(STDIN_FILENO, stdin_copy);
        stdin_change = 0;
    }
    if (stdout_change)
    {
        dup2(STDOUT_FILENO, stdout_change);
        stdout_change = 0;
    }
    if (stderr_change)
    {
        dup2(STDERR_FILENO, stderr_copy);
        stderr_change = 0;
    }
    if (cmd && cmd->redirects)
    {
        struct Redirects * rd = cmd->redirects;

    }
}
