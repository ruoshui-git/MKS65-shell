

## IMPLEMENTED FEATURES:

- all basic features
  - fork and exec commands
  - multiple commands on one line (separated by ";")
  - redirection < and >
  - pipes "|"

additional features (sorry for the delay):
- prompt with current directory
- parser features
  - "parse double quotes like this" (no support for ' ')
  - ignore  arbitrary     spaces
    - so: 'ls>out'=='ls > out'=='ls >out'=='ls> out'=='ls >    out'
  - no limit on number of commands nor the length of a single line
  - continuation of unterminated quotations
    - if a quote is unfinished on one line, parser will start a new line with ">" and ask for more inputs until another quotation mark is given
- redirect > and >> with file descriptor support
  - like: gcc 2>> out.log
  - multiple conflicting > redirects will resort to the last one, but creating all of the files
- handles redirects like 
  - "1> out.txt 2>&1"
  - "&> out.txt" (gets converted to "1> out.txt 2>&1")
- chaining multiple pipes
- ^C is captured and does nothing
- ^D is mapped to exit
- when running program, an optional second argument can be the file with commands to execute
  - ex: ./main test_cmds.txt
  - originally this was trying to ease the pain for Mr. DW because an error in the code caused program to unable to have commands redirected into it, so he would have to enter commands to manually test it; attempt to implement this feature fixed that bug
- no prompting when stdin is not a terminal session (file or redirection, either one)

## Attempted:
- Thought about using Readline library for some fancy features but ended up not having enough time to work on it

## BUGS:
- sometimes there will be a "broken pipe" when chaining too many pipes, probably due to the timing of the threads
- ^D in a program like cat will also cause shell to exit

## No support:
- no support for "[cmd] [num]< [file]", only "[cmd] < [file]"
  - i.e. you can only redirect in from stdin
- no support for "... [num]>>&[num] ...", only "[num]>&[num]", like "2>&1"
  - i.e. redirecting out to a fd only works for ">", not ">>"

## Credits:
- This solved one of my biggest problems on piping:
  - https://stackoverflow.com/questions/33884291/pipes-dup2-and-exec

## Files & Function Headers:
Note: please forgive me. Too many files and functions and you don't want all of them in one file but I'll try.
Each function is commented with good detail in each header file, so you might want to check that. Plus, all descriptions almost entirely copied from *.h files.

ast.c/h: functions and structs related to the syntax of the commands

        ```C
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
        ```

cmd_test.txt: some commands to test

cmds.c: implements shell's own commands (cd and exit); if new features are addes, things like "export" can also be added

        ```C
        /** 
         * Implements the "cd" command
        */
        void cd(const char * path);

        /** 
         * Implements the "exit" command
        */
        void shell_exit(void);
        
        ```


colors.h: constants for ANSI color codes

lexer.c: lex input; generated by flex from lexer.l
lexer.l: rules for lexer

main.c: register signal handlers and run shell

parser.c: use results from lexer and convert user input into syntax structure that the program can understand

        ```C
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

        /** Restart lexer on the same file */
        void restart_lexer();

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

        ```

processes.c: handles things related to processes, such as process forking, command execution, redirection, pipes, etc

        ```C
        /**
         * Handles redirect by setting system fd to the files
         * @arg cmd command that contains redirection
         * @return 0 on success, -1 on sys error, -2 on fn error
         */
        int handle_redirects(struct Redirect * rd);

        /** 
         * Convert from shell fd to sys std fd
         * @arg src_fileno src file descriptor
         * @return std fd
        */
        int get_std_fileno(int src_fileno);

        /**
         * Execute a Cmd, handles redirects but not pipes
         * @arg cmd command to be executed
        */
        void exec_cmd(struct Cmd * cmd);

        /** 
         * Wait for a particular child. Used in parent and useful for pipes.
         * @arg cpid child's pid
        */
        void wait_for(pid_t cpid);

        /** 
         * Handle pipes for a cmd
         * Fork children to execute commands and all of the piping
         * @arg cmd cmd to handle pipes for
        */
        void handle_pipes(struct Cmd *cmd);
        ```

shell.c: patch together the functions of a shell

        ```C
        /** 
         * The main shell program: prompt, parse, execute, repeat
         * Does not handle signals
         * @arg infile file to read commands from
        */
        void shell(FILE * infile);
        ```

utils.c/h: contains some helper functions and structs, including a simple queue implementation

        ```C
        // Linked List
        #ifndef LINKED_LIST
        #define LINKED_LIST
        /** 
         * A single node for pid linked list
        */
        struct PidNode
        {
            int data;
            struct PidNode *next;
        };

        /** 
         * A pid linked list
        */
        struct PidList
        {
            struct PidNode *head;
            struct PidNode *end;
            int len;
        };

        #endif

        /**
         * Print shell prompt
         */
        void prompt();

        /**
         * Clean up resources before exiting
         * Particularly useful in sighandler
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

        /** 
         * Append a new node to a linked list
         * @arg l the list to append node to
         * @arg data the number to be added
         * @note This function modifies the list
         * @return the linked list
        */
        struct PidList *lappend(struct PidList *l, int data);

        /** 
         * Remove a node from the list from the beginning and return the value
         * @warning Since the list is only used for storing pids, then negative numbers are not needed, so -1 is used for indication of error
         * @note This function modifies the list
         * 
         * @arg list linked list
         * @return value of the removed node, or -1 on error or empty list
        */
        int ldequeue(struct PidList * list);
        ```
