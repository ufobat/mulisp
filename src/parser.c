#include <string.h>
#include <stdlib.h>
#include "mulisp.h"


Object *make_symbol(char *val)
{
    Object *sym = malloc(sizeof(Object));
    sym->type = OTYPE_SYM;
    sym->str.value = malloc(sizeof(char) * (strlen(val) + 1));
    strncpy(sym->str.value, val, strlen(val) + 1);
    return sym;
}


int is_num_char(char c)
{
    return c >= '0' && c <= '9';
}

/* The following functions return 1 if the parse was successful, 0 otherwise.
 * The result of parsing is stored in the arguments
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

int parse_float(char *tok, double *value)
{
    int sign = 1;
    int integer = 0;
    double frac = 0;
    int divider = 10;
    int not_null = 0; // To avoid matching "." and "-."

    if (tok[0] == '-') {
        sign = -1;
        tok++;
    }
    if (!*tok)
        return 0;

    while (*tok) { // Integer part
        if (!is_num_char(*tok)) {
            if (*tok == '.') {
                tok++;
                break;
            }
            else
                return 0;
        }
        not_null = 1;
        integer *= 10;
        integer += (*tok) - '0';
        tok++;
    }
    while (*tok) { // Fractional part
        if (!is_num_char(*tok))
            return 0;
        not_null = 1;
        frac += (*tok - '0') / (float) divider;
        divider *= 10;
        tok++;
    }

    *value = sign * ((double) integer + frac);

    return not_null;
}

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


/* parse_list -- recursively parse a list, given the initial ( and some items have already been parsed
 *
 */
Object *parse_list(List **tokens_pointer)
{
    List *tokens = *tokens_pointer;
    char *first_token;
    Object *ret;
    Object *first, *rest;

    if (tokens == NULL)
        fatal_error("Non-terminated list\n");

    first_token = tokens->item;

    if (!strcmp(first_token, ")")) {
        free(first_token);
        *tokens_pointer = tokens->next;
        free(tokens);

        ret = nil;
    }
    else if (!strcmp(first_token, ".")) {
        free(first_token);
        *tokens_pointer = tokens->next;
        free(tokens);

        first = parse(tokens_pointer);
        tokens = *tokens_pointer;

        if (tokens == NULL) {
            fatal_error("Non-terminated dotted list\n");
        } else if (strcmp(tokens->item, ")")) {
            fatal_error("Expected ) instead of %s after .\n", (char *) tokens->item);
        } else {
            free(tokens->item);
            *tokens_pointer = tokens->next;
            free(tokens);
        }
        ret = first;
    }
    else {
        first = parse(tokens_pointer);
        rest = parse_list(tokens_pointer);
        ret = malloc(sizeof(Object));
        ret->type = OTYPE_PAIR;
        ret->pair.car = first;
        ret->pair.cdr = rest;
    }

    return ret;
}


/* parse -- returns the first object in tokens, and updates the tokens List pointer to point to the beginning of the
 * unparsed tokens. Also frees the parsed tokens and associated list.
 */
Object *parse(List **tokens_pointer)
{
    List *tokens = *tokens_pointer;
    Object *ret = malloc(sizeof(Object));
    char *first_token = tokens->item;
    int do_update_and_free = 1;

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

        do_update_and_free = 0;
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
            fprintf(stderr,
                    "Warning: extended chars not yet supported (parsing %s).\n",
                    first_token);

        ret->type = OTYPE_CHR;
        ret->chr.value = first_token[2];
    }
    else if (strlen(first_token) >= 2 && first_token[0] == '"' && first_token[strlen(first_token) - 1] == '"') {
        ret->type = OTYPE_STR;
        ret->str.length = (int) strlen(first_token) - 2;
        ret->str.value = malloc((ret->str.length + 2) * sizeof(char));
        strncpy(ret->str.value, first_token + 1, ret->str.length);
        ret->str.value[ret->str.length] = 0;
    }
    else if (parse_int(first_token, &(ret->integer.value))) {
        ret->type = OTYPE_INT;
    }
    else if (parse_float(first_token, &(ret->floating.value))) {
        ret->type = OTYPE_FLT;
    }
    else if (parse_frac(first_token, &(ret->fraction.numerator), &(ret->fraction.denominator))) {
        ret->type = OTYPE_FRAC;
    }
    else if (!strcmp(first_token, "(")) {
        free(ret);
        free(first_token);
        *tokens_pointer = tokens->next;
        free(tokens);

        ret = parse_list(tokens_pointer);
        do_update_and_free = 0;
    }
    else if (!strcmp(first_token, ")") || !strcmp(first_token, ".")) {
        fatal_error("Unmatched or unexpected %s\n", first_token);
    }
    else {
        free(ret);
        ret = make_symbol(first_token);
    }

    if (do_update_and_free) {
        free(first_token);
        *tokens_pointer = tokens->next;
        free(tokens);
    }

    return ret;
}
