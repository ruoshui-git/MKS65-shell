#include <stdio.h>
#include "ast.h"

/**
 * Return option for parser
 * @member status: 0 = EOL, 1 = To Continue, -1 = Error
 */
struct CmdOption
{
    int status;
    struct Cmd * cmd;
};

enum TOKENS {
    WORD = 260,
    QUOTED_WORD = 261,
    RDRT_READ = 262,
    RDRT_WRITE = 263,
    RDRT_APPEND = 264,
    PIPE = 265,
    SEMICOLON = 266,
    END = 0
};

/** Flex in a shell command and then parse it */
struct CmdOption readCmd();

/** Lex till the end of line or EOF */
void skip_to_end(void);

/** Old: parse arguments, return argv */
char **parse_args(char *line);

/** Restart lexer on a file pointer */
void restart_lexer(FILE * infile);
