%option noyywrap
%x STR
%{
#define MAX_STR_CONST 1024

#include "parser.h"

void yyerror(char * msg);

char str_buf[MAX_STR_CONST];
char * str_buf_ptr;
%}


%%


\" { str_buf_ptr = str_buf; BEGIN(STR); }

<STR>
{
    "\""    {
        BEGIN(INITIAL);
        *str_buf_ptr = '\0';
        /* fprintf(yyout, " \"%s\" ", str_buf); */
        return QUOTED_WORD;
    }

    \n {
        *str_buf_ptr++ = '\n';
        fputs("> ", yyout);
    }

    \\[0-7]{1,3} {
        /* octal escape sequence */
        int result;
        (void) sscanf( yytext + 1, "%o", &result );

		   if ( result > 0xff )
			   /* error, constant is out-of-bounds */

		   *str_buf_ptr++ = result;
    }

    \\[0-9]+ {
        yyerror("bad escape sequence");
    }

    \\n  { *str_buf_ptr++ = '\n'; }
    \\t  { *str_buf_ptr++ = '\t'; }
    \\r  { *str_buf_ptr++ = '\r'; }
    \\b  { *str_buf_ptr++ = '\b'; }
    \\f  { *str_buf_ptr++ = '\f'; }

    \\(.\n) { *str_buf_ptr++ = yytext[1]; }

    [^\\\n\"]+ {
        char * yptr = yytext;
        while ( * yptr )
        {
            *str_buf_ptr++ = *yptr++;
        }
    }
}

;                        { /* puts("");                                            */    return SEMICOLON; }
([0-9]+)?"<"             { /* fprintf(yyout, " <redirect read> ");                 */    return RDRT_READ; }
([0-9]+|&)?">"           { /* fprintf(yyout, " <redirect ovewrite: %s> ", yytext); */    return RDRT_WRITE; }
([0-9]+|&)?">>"          { /* fprintf(yyout, " <redirect append: %s> ", yytext);   */    return RDRT_APPEND; }
"|"                      { /* fputs(" <pipe> ", yyout);                            */    return PIPE; }
[^ \t\n\f\r\v;\"<>|]+    { /* printf(" \"%s\" ", yytext);                          */    return WORD; }

[ \t] { }

\n                       {                                                               return EOL; }
<<EOF>>                  { /* fputs(yyout, "EOF, exiting\n") */                          return END; }


%%

// void parse(void)
// {
//     yylex();
// }

// int main(void)
// {
//     parse();
//     return 0;
// }

void yyerror(char * msg)
{
    printf("Scanner error: %s\n", msg);
}
