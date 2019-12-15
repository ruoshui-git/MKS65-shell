#include "ast.h"

/**
 * Handles redirect by setting system fd to the files
 * @arg cmd
 * @return 0 on success, -1 on sys error, -2 on fn error
 */
int handle_redirects(struct Redirect * rd);

/** 
 * Convert from shell fd to sys std fd
 * @arg src_fileno src fd
 * @return std fd
*/
int get_std_fileno(int src_fileno);