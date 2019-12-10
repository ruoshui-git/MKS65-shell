char **parse_args(char *line);
void pserror(char * msg);


enum TOKENS {
    WORD = 260,
    QUOTED_WORD = 261,
    RDRT_READ = 262,
    RDRT_WRITE = 263,
    RDRT_APPEND = 264,
    PIPE = 265,
    SEMICOLON = 266
};
