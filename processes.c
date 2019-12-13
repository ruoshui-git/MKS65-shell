#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "cmds.h"
#include "ast.h"


int handle_redirects(struct Redirect * rd)
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
            dup2(fd, STDIN_FILENO);
        }
        else
        {
            int flag;
            if (type == 1)
            {
                flag = O_TRUNC;
            }
            else
            {
                // type == 2
                flag = O_APPEND;
            }
            
            int fd = open(rd->filename, O_WRONLY | O_CREAT  | __O_CLOEXEC | flag);
            if (fd == -1)
            {
                perror("open");
                return -1;
            }
            int std_fileno;
            int src_fileno = rd->src_fileno;
            if (src_fileno == 0)
            {
                std_fileno = STDIN_FILENO;
            }
            else if (src_fileno == 1)
            {
                std_fileno = STDOUT_FILENO;
            }
            else if (src_fileno == 2)
            {
                std_fileno = STDERR_FILENO;
            }
            int d_result = dup2(fd, std_fileno);
            if (d_result == -1)
            {
                perror("dup2");
                return -1;
            }
        }
        rd = rd->next;
    }
    return 0;
}