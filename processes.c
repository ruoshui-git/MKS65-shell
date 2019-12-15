#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "cmds.h"
#include "ast.h"

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
            dup2(fd, STDIN_FILENO);
        }
        else
        {
            // before, type of redirect separates ctl flow; now we separate based on file starts with "&" or not
            // a file that starts with "&" mark a fileno, not an actual file
            if (rd->filename[0] == '&')
            {
                // convert the rest to a file descriptor
                int dest_fileno = atoi(&(rd->filename[1]));
                int d_result = dup2(dest_fileno, rd->src_fileno);
                if (d_result == -1)
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