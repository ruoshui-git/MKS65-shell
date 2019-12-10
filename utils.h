/**
 * Count the occurence of a char in a string
 */
int countchar(char * src, char c);

/**
 * Print shell prompt
 */
void prompt();

/**
 * Clean up resources before exiting
 */
void cleanup();

/** Keep track of arguments */
extern char ** argv;

/** Keep track of current line of input */
extern char * ln;

/** Print shell error */
void serror(char * msg);

/** Print parser error */
void pserror(char * msg);

/** Print internal shell error */
void iserror(char * msg);