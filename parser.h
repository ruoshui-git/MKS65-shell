#include <stdio.h>
#include "ast.h"

/**
 * Return option for parser
 * @member status: 0 = EOL, 1 = To Continue, -1 = Error, -2 = EOF
 */
struct CmdOption
{
    int status;
    struct Cmd *cmd;
};

/** Tokens used in lexer and parser */
enum TOKENS
{
    WORD = 260,
    QUOTED_WORD = 261,
    RDRT_READ = 262,
    RDRT_WRITE = 263,
    RDRT_APPEND = 264,
    PIPE = 265,
    SEMICOLON = 266,
    EOL = 267,
    END = 268
};

/** Flex in a shell command and then parse it */
struct CmdOption readCmd();

/** Lex till the end of line or EOF */
void skip_to_end(void);

/** Old: parse arguments, return argv */
char **parse_args(char *line);

/** Restart lexer on a file pointer */
void restart_lexer(FILE *infile);

/** 
 * Get the fileno from a redirect operator
 * Ex: return 2 on "2>"
 * @arg rd_text string representing the redirect operation
 * @return fileno
 */
int get_rd_fileno(char *rd_text);

/** 
 * Call yylex() to read in a file. Used for redirects.
 * @arg file_ptr will point to the file
 * @return status
*/
int lex_rd_file(char **file_ptr);
