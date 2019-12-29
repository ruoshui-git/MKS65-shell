#include "ast.h"

/** Named constants for pipe access */
#define PIPE_READ 0
#define PIPE_WRITE 1

/**
 * Handles redirect by setting system fd to the files
 * @arg cmd command that contains redirection
 * @return 0 on success, -1 on sys error, -2 on fn error
 */
int handle_redirects(struct Redirect * rd);

/** 
 * Convert from shell fd to sys std fd
 * @arg src_fileno src file descriptor
 * @return std fd
*/
int get_std_fileno(int src_fileno);

/**
 * Execute a Cmd, handles redirects but not pipes
 * @arg cmd command to be executed
*/
void exec_cmd(struct Cmd * cmd);

/** 
 * Wait for a particular child. Used in parent and useful for pipes.
 * @arg cpid child's pid
*/
void wait_for(pid_t cpid);

/** 
 * Handle pipes for a cmd
 * Fork children to execute commands and all of the piping
 * @arg cmd cmd to handle pipes for
*/
void handle_pipes(struct Cmd *cmd);