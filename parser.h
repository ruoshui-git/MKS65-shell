char **parse_args(char *line);

struct WordElem
{
    char * word;
    struct WordElem * next;
};

struct WordList
{
    struct WordElem * head;
    int len;
};

struct Cmd
{
    struct WordList * cmd;
    int cin;
    int cout;
    int cerr;
};

struct CmdElem
{
    struct Cmd * cmd;
    struct CmdElem * next;
};

struct CmdList
{
    struct CmdElem * head;
    int len;
};

