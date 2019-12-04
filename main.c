#include <stdio.h>
#include <stdlib.h>

const int ln_len = 1000;
int RUNNING = 1;

void prompt();

int main(void)
{
    char * ln = malloc(ln_len * sizeof(char));
    while (RUNNING)
    {
        prompt();
        fgets(ln, ln_len, stdin);
        printf("%s", ln);
        // do stuff
    }
    // parse(ln, parsed_ln);
    // exec(parsed_ln);
}

void prompt()
{
    fputs(" # ", stdout);
}
// void parse();
// void exec();
