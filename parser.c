#include <string.h>
#include "mulisp.h"

Object *parse_list(List **tokens_pointer, int free);


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
        frac += (*tok - '0') / (float)divider;
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
            if(*tok == '/') {
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

    *num_ptr = sign*num;
    *denom_ptr = denom;

    return num_not_null && denom_not_null;
}

/* parse -- returns the first object in tokens, and updates the tokens List pointer to point to the beginning of the
 * unparsed tokens. If free is set, frees the parsed tokens and associated list.
 */
Object *parse(List **tokens_pointer, int free)
{
    List *tokens = *tokens_pointer;
    Object *ret = malloc(sizeof(Object));
    char *first_token = tokens->item;

    if (!strcmp(first_token, "()")) {
        ret->type = OTYPE_NIL;
    }
    else if (!strcmp(first_token, "#t")) {
        ret->type = OTYPE_BOOL;
        ret->boolean.value = 1;
    }
    else if (!strcmp(first_token, "#f")) {
        ret->type = OTYPE_BOOL;
        ret->boolean.value = 0;
    }
    else if (!strncmp(first_token, "#\\", 2)) {
        if (strlen(first_token) < 3)
            fatal_error("%s is an invalid symbol\n", first_token);
        if (strlen(first_token) > 3)
            fprintf(stderr, "Warning: extended chars not yet supported (parsing %s)\n", first_token);

        ret->type = OTYPE_CHR;
        ret->chr.value = first_token[2];
    }
    else if (strlen(first_token) >= 2 && first_token[0] == '"' && first_token[strlen(first_token) - 1] == '"') {
        ret->type = OTYPE_STR;
        ret->str.length = (int) strlen(first_token) - 2;
        ret->str.value = malloc((ret->str.length + 1) * sizeof(int));
        strncpy(ret->str.value, first_token + 1, ret->str.length);
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
    else {
        fatal_error("%s: unknown type", first_token);
    }

    return ret;
}