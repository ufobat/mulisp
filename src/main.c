#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <zos_sys.h>
#include <zos_vfs.h>

#include "mulisp.h"

#define MAX_LINE 100

void repl(zos_dev_t input_file, bool verbose)
{
    char line[MAX_LINE];
    List *tokens = NULL;
    Object *parsed;
    char prompt[5] = ">>> ";
    uint16_t size;
    zos_err_t err;

    while(1) {
        if (verbose) {
            size = strlen(prompt);
            err = write(DEV_STDOUT, prompt, &size);
            if(err != ERR_SUCCESS) {
                printf("ERROR: stdout, prompt %02\n", err);
                exit(err);
            }
        }

        size = MAX_LINE;
        err = read(input_file, line, &size);
        if(err != ERR_SUCCESS) {
            printf("ERROR: fgets %02\n", err);
            exit(err);
        }

        if (size < 2) {
            break;
        }
        if (!strcmp(line, ",exit")) {
            break;
        }

        tokens = append_list(tokens, tokenize(line));
        while(tokens != NULL) {
            parsed = parse_and_free(&tokens);
            if (parsed == NULL) {
                strcpy(prompt, "... ");
                break;
            } else {
                strcpy(prompt, ">>> ");
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
    (void)argc;
    (void)argv;

    zos_dev_t input_file = DEV_STDIN;
    bool verbose = true;

    init();

    printf("*** muScheme %s\n", VERSION_STRING);

    // TODO: parse args
    // if (argc == 2) {
    //     input_file = fopen(argv[1], "r");
    //     if (input_file == NULL) {
    //         printf("Could not open file %s\n", argv[1]);
    //         exit(1);
    //     }
    //     verbose = false;
    // } else if (argc > 2) {
    //     printf("Usage: %s (input_file)\n", argv[0]);
    //     exit(1);
    // }

    repl(input_file, verbose);

    return 0;
}
