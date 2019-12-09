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
 * A command, which has its own in, out, and error sudofiles
 */
struct Cmd
{
    struct WordList * cmd;
    int cin;
    int cout;
    int cerr;
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

/**
 * Append a WordElem e to an existing WordList wl. If wl is NULL, and create a new wl.
 * @return address of wl
 */
struct WordList * wl_append(struct WordList * wl, struct WordElem * e);

/**
 * @return a new WordElem with string str
 */
struct WordElem * make_word(char * str);