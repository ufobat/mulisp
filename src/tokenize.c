/**
 * \file tokenize.c
 * Tokenization functions
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "mulisp.h"

#define MAX_TOKEN_LENGTH 1000

/**
 * one_char_sym returns true iff its parameter is a one-character symbol that
 * behaves as if there were spaces around it.
 * @param c The char to check
 * @return
 */
int one_char_sym(int c)
{
    return c == '(' || c == ')' || c == '\'';
}

int part_of_symbol(int c)
{
    return c && !(isspace(c) || c == '(' || c == ')');
}


/**
 * next_tok_from returns the next token in a string, updating the argument to
 * the first position after the end of the returned token.
 * TODO: add support for long chars like #\space
 * @param string_pointer A pointer to the string from which to get a token
 * @return The first token from the string, newly allocated.
 */
char *next_tok_from(char **string_pointer)
{
    char *token = malloc(sizeof(char) * MAX_TOKEN_LENGTH);
    char *string = *string_pointer;

    // Skip initial blanks
    if (isspace(*string))
        while (isspace(*(++string)));

    // String: match corresponding "
    if (*string == '"') {
        char *matching = strchr(string + 1, '"');
        if (matching == NULL)
            fatal_error("Unmatched \"\n");
        else if (matching - string > MAX_TOKEN_LENGTH - 2)
            fatal_error("String constant over %d length\n",
                        MAX_TOKEN_LENGTH - 2);

        strncpy(token, string, matching - string + 1);
        token[matching - string + 1] = 0;
        string = matching + 1;
    }

        // Comment: skip to newline and call recursively
    else if (*string == ';') {
        char *end = strchr(string + 1, '\n');

        free(token);

        if (end == NULL) {
            return NULL;
        }
        string = end;
        token = next_tok_from(&string);
    }

        // One-char symbol
    else if (one_char_sym(*string)) {
        token[0] = *string;
        token[1] = 0;
        string++;
    }

        // Normal symbol
    else if (*string) {
        int i_token = 0;

        do {
            token[i_token] = *string;
            i_token++;
            string++;
        } while (*string && part_of_symbol(*string) &&
                 i_token < MAX_TOKEN_LENGTH - 1);

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


/**
 * tokenize returns a list of token in the corresponding string.
 * @param string The string to tokenize
 * @return A list of tokens
 */
List *tokenize(char *string)
{
    List *current_lst_item = NULL;
    List *lst_head = NULL;
    char *new_token;

    while (*string != 0) {
        new_token = next_tok_from(&string);
        if (new_token == NULL)
            break;

        if (lst_head == NULL) {
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
