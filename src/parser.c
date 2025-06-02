/**
 * \file parser.c
 * Functions for parsing the input
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "mulisp.h"


/**
 * make_symbol creates a new symbol whose value is a given string.
 * @param val The value of the symbol. This string is copied into the symbol,
 * and can safely be modified after calling make_symbol.
 * @return The newly-created object
 */
Object *make_symbol(char *val)
{
    Object *sym = malloc(sizeof(Object));
    sym->type = OTYPE_SYM;
    sym->str.value = malloc(sizeof(char) * (strlen(val) + 1));
    strncpy(sym->str.value, val, strlen(val) + 1);
    return sym;
}


/**
 * is_num_char checks that a char is numeric.
 * @param c The char to check
 * @return 1 if c is numeric, 0 otherwise
 */
int is_num_char(char c)
{
    return c >= '0' && c <= '9';
}


/**
 * parse_int tries to parse a token into an integer
 * @param tok The token to parse
 * @param value A pointer that will be set to the integer value
 * @return 1 if an integer was successfully parsed, 0 otherwise
 */
int parse_int(char *tok, int *value)
{
    int val = 0;
    int sign = 1;

    if (tok[0] == '-') {
        sign = -1;
        tok++;
    }
    if (!*tok) // It's okay to check this here
        return 0;

    while (*tok) {
        if (!is_num_char(*tok))
            return 0;
        val *= 10;
        val += (*tok) - '0';
        tok++;
    }

    *value = val * sign;

    return 1;
}

/**
 * parse_float tries to parse a token into a float
 * @param tok The token to parse
 * @param value A pointer that will be set to the float value
 * @return 1 if a float was successfully parsed, 0 otherwise
 */
// int parse_float(char *tok, float *value)
// {
//     int sign = 1;
//     int integer = 0;
//     float frac = 0;
//     int divider = 10;
//     int not_null = 0; // To avoid matching "." and "-."

//     if (tok[0] == '-') {
//         sign = -1;
//         tok++;
//     }
//     if (!*tok)
//         return 0;

//     while (*tok) { // Integer part
//         if (!is_num_char(*tok)) {
//             if (*tok == '.') {
//                 tok++;
//                 break;
//             }
//             else
//                 return 0;
//         }
//         not_null = 1;
//         integer *= 10;
//         integer += (*tok) - '0';
//         tok++;
//     }
//     while (*tok) { // Fractional part
//         if (!is_num_char(*tok))
//             return 0;
//         not_null = 1;
//         frac += (*tok - '0') / (float) divider;
//         divider *= 10;
//         tok++;
//     }

//     *value = sign * ((float) integer + frac);

//     return not_null;
// }

/**
 * parse_frac tries to parse a token into a fraction
 * @param tok The token to parse
 * @param num_ptr A pointer that will be set to the parsed numerator
 * @param denom_ptr A pointer that will be set to the parsed denumerator
 * @return 1 if a fraction was successfully parsed, 0 otherwise.
 */
int parse_frac(char *tok, int *num_ptr, unsigned *denom_ptr)
{
    int num = 0;
    unsigned denom = 0;
    int sign = 1;
    int num_not_null = 0, denom_not_null = 0;

    if (tok[0] == '-') {
        sign = -1;
        tok++;
    }
    if (!*tok)
        return 0;

    while (*tok) {
        if (!is_num_char(*tok)) {
            if (*tok == '/') {
                tok++;
                break;
            }
            else
                return 0;
        }
        num_not_null = 1;
        num *= 10;
        num += (*tok) - '0';
        tok++;
    }
    while (*tok) { // Fractional part
        if (!is_num_char(*tok))
            return 0;
        denom_not_null = 1;
        denom *= 10;
        denom += (*tok - '0');
        tok++;
    }

    *num_ptr = sign * num;
    *denom_ptr = denom;

    return num_not_null && denom_not_null;
}

Object *parse(List **tokens_pointer);

/**
 * parse_list recursively parses a list, given that the initial '(' and
 * possibly some elements have been parsed.
 * @param tokens_pointer A pointer to the tokens list, which will be updated
 * to remove the successfully parsed elements.
 * @return The parsed list, or `NULL` if the list was not terminated.
 */
Object *parse_list(List **tokens_pointer)
{
    List *tokens = *tokens_pointer;
    char *first_token;
    Object *ret;
    Object *first, *rest;

    if (tokens == NULL)
        return NULL;

    first_token = tokens->item;

    if (!strcmp(first_token, ")")) {
        *tokens_pointer = tokens->next;

        ret = nil;
    }
    else if (!strcmp(first_token, ".")) {
        *tokens_pointer = tokens->next;

        first = parse(tokens_pointer);
        tokens = *tokens_pointer;

        if (tokens == NULL) {
            return NULL;
        } else if (strcmp(tokens->item, ")")) {
            fatal_error("Expected ) instead of %s after .\n", (char *) tokens->item);
        } else {
            *tokens_pointer = tokens->next;
        }
        ret = first;
    }
    else {
        first = parse(tokens_pointer);
        rest = parse_list(tokens_pointer);

        if (rest == NULL) {
            return NULL;
        }

        ret = malloc(sizeof(Object));
        ret->type = OTYPE_PAIR;
        ret->pair.car = first;
        ret->pair.cdr = rest;
    }

    return ret;
}


/**
 * parse returns the first object in tokens, and updates the tokens List
 * pointer to point to the beginning of the unparsed tokens.
 * @param tokens_pointer A pointer to the tokens list, which will be updated
 * to remove the successfully parsed elements.
 * @return The parsed object, or NULL if no object could be parsed
 * successfully.
 */
Object *parse(List **tokens_pointer)
{
    List *tokens = *tokens_pointer;

    if (tokens == NULL) {
        return NULL;
    }

    Object *ret = malloc(sizeof(Object));
    char *first_token = tokens->item;
    *tokens_pointer = tokens->next;

    if (!strcmp(first_token, "'")) {
        ret->type = OTYPE_PAIR;

        ret->pair.car = malloc(sizeof(Object));
        ret->pair.car->type = OTYPE_SYM;
        ret->pair.car->str.value = malloc(strlen(QUOTE_STRING) + 1);
        strcpy(ret->pair.car->str.value, QUOTE_STRING);

        ret->pair.cdr = malloc(sizeof(Object));
        ret->pair.cdr->type = OTYPE_PAIR;
        ret->pair.cdr->pair.cdr = nil;

        *tokens_pointer = tokens->next;
        ret->pair.cdr->pair.car = parse(tokens_pointer);
    } else if (!strcmp(first_token, "#t")) {
        ret->type = OTYPE_BOOL;
        ret->boolean.value = 1;
    }
    else if (!strcmp(first_token, "#f")) {
        ret->type = OTYPE_BOOL;
        ret->boolean.value = 0;
    }
    else if (!strncmp(first_token, "#\\", 2)) {
        if (strlen(first_token) < 3)
            fatal_error("%s is an invalid symbol.\n", first_token);
        if (strlen(first_token) > 3)
            printf("Warning: extended chars not yet supported (parsing %s).\n", first_token);

        ret->type = OTYPE_CHR;
        ret->chr.value = first_token[2];
    }
    else if (strlen(first_token) >= 2 && first_token[0] == '"' &&
            first_token[strlen(first_token) - 1] == '"') {
        ret->type = OTYPE_STR;
        ret->str.length = (int) strlen(first_token) - 2;
        ret->str.value = malloc((ret->str.length + 2) * sizeof(char));
        strncpy(ret->str.value, first_token + 1, ret->str.length);
        ret->str.value[ret->str.length] = 0;
    }
    else if (parse_int(first_token, &(ret->integer.value))) {
        ret->type = OTYPE_INT;
    }
    // else if (parse_float(first_token, &(ret->floating.value))) {
    //     ret->type = OTYPE_FLT;
    // }
    else if (parse_frac(first_token, &(ret->fraction.numerator),
                        &(ret->fraction.denominator))) {
        ret->type = OTYPE_FRAC;
    }
    else if (!strcmp(first_token, "(")) {
        free(ret);

        ret = parse_list(tokens_pointer);

        if (ret == NULL) {
            return NULL;
        }
    }
    else if (!strcmp(first_token, ")") || !strcmp(first_token, ".")) {
        fatal_error("Unmatched or unexpected %s\n", first_token);
    }
    else {
        free(ret);
        ret = make_symbol(first_token);
    }

    return ret;
}


/**
 * parse_and_free calls parse, then frees the parsed part of the token list
 * if the parse was successful, or restores the tokens pointer to its original
 * status otherwise.
 * @param tokens_pointer A pointer to a list of tokens
 * @return The parsed object
 */
Object *parse_and_free(List **tokens_pointer)
{
    List *initial_tokens = *tokens_pointer;

    Object *ret = parse(tokens_pointer);

    if (ret == NULL) {
        *tokens_pointer = initial_tokens;
        return NULL;
    }

    for (List *token = initial_tokens;
         token != NULL && token != *tokens_pointer;) {
        List *prev_token = token;
        free(token->item);
        token = token->next;
        free(prev_token);
    }

    return ret;
}
