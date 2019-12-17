
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
 * A command, which has its own in, out, and error sudofiles
 *
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
 * Append a WordElem e to an existing WordList wl. If wl is NULL, and create a new wl.
 * @return address of wl
 */
struct WordList * wl_append(struct WordList * wl, struct WordElem * e);

/**
 * @return a new WordElem with string str
 */
struct WordElem * make_word(char * str);

struct Cmd * make_cmd(struct WordList * wl);

struct Cmd *cmd_append_redirect(struct Cmd *cmd, int rdrt_type, int src_fileno, char *file);

/** Make a argv for execvp from a struct Cmd */
char ** cmd_to_argv(struct Cmd * cmd);

struct Cmd * clear_cmd(struct Cmd * cmd);
struct Cmd * free_cmd(struct Cmd * cmd);
struct WordList * free_word_list(struct WordList * wl);
struct Redirect * free_redirects(struct Redirect * rd);
struct Cmd * attach_pipe(struct Cmd * cmd, struct Cmd * pipeto);