#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mulisp.h"
#include "zos_vfs.h"
#include "zos_sys.h"
#include "zeal_linereader.h" // Include the header for fgets

#define MAX_LINE 1000

void repl(zos_dev_t *input_file, bool verbose)
{
    char line[MAX_LINE];
    List *tokens = NULL;
    Object *parsed;
    char *prompt = ">>> ";

    printf("foo");
    printf(prompt);
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
            write_newline(eval(parsed, global_environment));
        }
    }
}

int main(int argc, char **argv)
{
    zos_dev_t input_file = DEV_STDIN;
    bool verbose = true;

    init();

    printf("*** muScheme %s\n", VERSION_STRING);

    /*
    if (argc == 1) {
        input_file = open(argv[0], O_RDONLY);
        if (input_file == NULL) {
            printf("Could not open file %s\n", argv[0]);
            exit(1);
        }
        verbose = false;
    } else if (argc >= 2) {
        printf("Usage: mulisp (input_file)\n");
        
        */

    repl(input_file, true);
    return 0;
}
