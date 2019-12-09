%option noyywrap
%x STR
%{
#define MAX_STR_CONST 1024

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
        fprintf(yyout, " \"%s\" ", str_buf);
        /* return val to parser here!!!!! */
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

; { puts(""); }
[0-9]?"<" { fprintf(yyout, " <direct left> "); }
([0-9]|&)?">" { fprintf(yyout, " <overwrite: %s> ", yytext); }
([0-9]|&)?">>" { fprintf(yyout, " <append: %s> ", yytext); }
"|" { fputs(" <pipe> ", yyout); }

[^ \t\n\f\r\v;\"<>|]+ { printf(" \"%s\" ", yytext); }


[ \t] { }
%%

void parse(void)
{
    yylex();
}

int main(void)
{
    parse();
    return 0;
}

void yyerror(char * msg)
{
    printf("Scanner error: %s\n", msg);
}