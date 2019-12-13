#include "ast.h"

/**
 * Handles redirect by setting system fd to the files
 * @arg cmd
 * @return 0 on success, -1 on sys error, -2 on fn error
 */
int handle_redirects(struct Redirect * rd);