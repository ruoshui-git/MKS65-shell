#ifndef AST_STRUCTS
#define AST_STRUCTS

/**
 * An element of a WordList
 */
struct WordElem
{
    char * word;
    struct WordElem * next;
    struct WordElem * prev;
};

/**
 * A list of words, used to form a command
 */
struct WordList
{
    struct WordElem * head;
    struct WordElem * end;
    int len;
};

/**
 * Store information about a redirection, attached to a command
 * @member src_fileno descriptor to be modified: 0 = stdin, 1 = stdout, 2 = stderr
 * @member filename the file that should be directed to
 * @member type type of redirection: 0 = <, 1 = >, 2 = >>
 */
struct Redirect
{
    int type;
    int src_fileno;
    char * filename;
    struct Redirect * next;
};

/**
 * A command, which has its own redirects and pipes
 */
struct Cmd
{
    struct WordList * cmd;
    struct Redirect * redirects;
    struct Cmd * pipeto;
};

/**
 * An element in a CmdList
 */
struct CmdElem
{
    struct Cmd * cmd;
    struct CmdElem * next;
};

/**
 * A list of commands, useful for parsing multiple commands.
 * This is also the root of this shell's AST.
 */
struct CmdList
{
    struct CmdElem * head;
    int len;
};

#endif

/**
 * Append a WordElem e to an existing WordList wl
 * If wl is NULL, and create a new wl
 * @return address of wl
 */
struct WordList * wl_append(struct WordList * wl, struct WordElem * e);

/**
 * @arg str value for this WordElem
 * @return a new WordElem with string str
 */
struct WordElem * make_word(char * str);

/** 
 * @arg wl words for this command
 * @return a new Cmd with WordList wl
*/
struct Cmd * make_cmd(struct WordList * wl);

/** 
 * Append a redirection to a command
 * @arg cmd command to append redirection to
 * @arg rdrt_type 0 for "<", 1 for ">", 2 for ">>"
 * @arg src_fileno which one of the standard file descriptors to change (1=stdin, 2=stdout, or 3=stderr)
 * @arg file filename of the file for redirection
 * @return cmd with the redirection
*/
struct Cmd *cmd_append_redirect(struct Cmd *cmd, int rdrt_type, int src_fileno, char *file);

/** 
 * @arg cmd Cmd for conversion
 * @return argument vector (array) for execvp made from a cmd
*/
char ** cmd_to_argv(struct Cmd * cmd);

/** 
 * Clear (free) all fields of the cmd but not freeing it
 * @arg cmd command to be cleared
 * @return an empty cmd
*/
struct Cmd * clear_cmd(struct Cmd * cmd);

/** 
 * Clear the cmd and then free the cmd
 * @arg cmd command to be (cleared and then) freed
 * @return NULL
*/
struct Cmd * free_cmd(struct Cmd * cmd);

/** 
 * Free a WordList and the WordElem-s in it
 * @return NULL
*/
struct WordList * free_word_list(struct WordList * wl);

/** 
 * Free redirects
 * @return NULL
*/
struct Redirect * free_redirects(struct Redirect * rd);

/** 
 * Attach a pipe to the command
 * @arg cmd command to be modified to attach pipe
 * @arg pipeto a command to be piped to from the current command
 * @return cmd with pipe attached
*/
struct Cmd * attach_pipe(struct Cmd * cmd, struct Cmd * pipeto);