#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mulisp.h"


void repl(FILE *input_file, bool verbose)
{
    const int MAX_LINE = 1000;
    char line[MAX_LINE];
    List *tokens = NULL;
    Object *parsed;
    char *prompt = ">>> ";

    while(1) {
        if (verbose)
            printf(prompt);

        if (fgets(line, MAX_LINE, input_file) == NULL)
            break;
        if (!strcmp(line, ",exit"))
            break;

        tokens = append_list(tokens, tokenize(line));
        while(tokens != NULL) {
            parsed = parse_and_free(&tokens);
            if (parsed == NULL) {
                prompt = "... ";
                break;
            } else {
                prompt = ">>> ";
            }

            if (verbose) {
                write_item(parsed);
                printf("==>\n");
            }
            write(eval(parsed, global_environment));
        }
    }
}

int main(int argc, char **argv)
{
    FILE *input_file = stdin;
    bool verbose = true;

    init();

    printf("*** muScheme %s\n", VERSION_STRING);

    if (argc == 2) {
        input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
            printf("Could not open file %s\n", argv[1]);
            exit(1);
        }
        verbose = false;
    } else if (argc > 2) {
        printf("Usage: %s (input_file)\n", argv[0]);
        exit(1);
    }

    repl(input_file, verbose);

    return 0;
}
