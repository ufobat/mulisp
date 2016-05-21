#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mulisp.h"

#define MAX_TOKEN_LENGTH 1000

int one_char_sym(int c) {
    return c == '(' || c == ')';
}

int part_of_symbol(int c) {
    return c && !(isspace(c) || c == '(' || c == ')');
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

    // String: match corresponding "
    if(*string == '"') {
        char* matching = strchr(string+1, '"');
        if(matching == NULL)
            fatal_error("Unmatched \"\n");
        else if(matching - string > MAX_TOKEN_LENGTH-2)
            fatal_error("String constant over %d length\n", MAX_TOKEN_LENGTH-2);

        strncpy(token, string, matching-string+1);
        string = matching+1;
    }

    // Comment: skip to newline and call recursively
    else if(*string == ';') {
        char* end = strchr(string+1, '\n');

        free(token);

        if(end == NULL) {
            return NULL;
        }
        string = end;
        token = next_tok_from(&string);
    }

    // One-char symbol
    else if(one_char_sym(*string)) {
        token[0] = *string;
        token[1] = 0;
        string++;
    }

    // Normal symbol
    else if(*string){
        int i_token = 0;

        do {
            token[i_token] = *string;
            i_token++;
            string++;
        } while(*string && part_of_symbol(*string) && i_token < MAX_TOKEN_LENGTH-1);

        token[i_token] = 0;
    }

    // End of string
    else {
        free(token);
        token = NULL;
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
