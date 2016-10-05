#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mulisp.h"


void repl()
{
    const int MAX_LINE = 1000;
    char line[MAX_LINE];
    List *tokens;
    Object *parsed;

    while(1) {
        printf("> ");
        fgets(line, MAX_LINE, stdin);
        if(!strncmp(line, "_exit", 5))
            break;

        tokens = tokenize(line);
        while(tokens != NULL) {
            parsed = parse(&tokens);
            write_item(parsed);
            printf("==>\n");
            write(eval(parsed, global_environment));
        }
    }
}

int main()
{
    init();

    printf("*** muScheme %s\n", VERSION_STRING);

    repl();

    return 0;
}
