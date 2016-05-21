#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mulisp.h"

#define MAX_TOKEN_LENGTH 1000

int part_of_symbol(int c) {
    return !(isspace(c) || c == '(' || c == ')' || c == '"' || c == EOF);
}

int skip_blanks(FILE* stream)
{
    int c;

    do {
        c = getc(stream);
        if(c == EOF)
            break;
    } while(isspace(c));

    ungetc(c, stream);

    return 0;
}


/* next_tok_from - returns the next token in string. Updates string to the first position after the end of the
 * returned token.
 */
char* next_tok_from(char** string_pointer) {
    char* token = malloc(sizeof(char) * MAX_TOKEN_LENGTH);
    char* string = *string_pointer;

    // Skip initial blanks
    if(isspace(*string))
        while(isspace(*(++string)));

    // String: match to corresponding "
    if(*string == '"') {
        char* matching = strchr(string+1, '"');
        if(matching == NULL || matching - string > MAX_TOKEN_LENGTH-2)
            fatal_error("Unmatched \" or string constant over %d\n", MAX_TOKEN_LENGTH-2);

        strncpy(token, string, matching-string);
    }
    // Comment: skip to newline and call recursively
    else if(*string == ';') {
        char* end = strchr(string+1, '\n');
        if(end == NULL) {
            free(token);
            return NULL;
        }


    }

    *string_pointer = string;

    return token;
}


List* tokenize(char* string) {
    List * lst_head = NULL;
    List * current_lst_item;
    char * new_token;

    while(*string != 0) {
        new_token = next_tok_from(&string);
        if(new_token == NULL)
            break;

        if(lst_head == NULL) {
            lst_head = malloc(sizeof(List));
            current_lst_item = lst_head;
            current_lst_item->item = new_token;
            current_lst_item->next = NULL;
        } else {
            current_lst_item->next = malloc(sizeof(List));
            current_lst_item = current_lst_item->next;
            current_lst_item->item = new_token;
            current_lst_item->next = NULL;
        }
    }

    return lst_head;
}
