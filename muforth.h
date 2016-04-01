//
// Created by Matthieu Felix on 01/04/2016.
//

#ifndef MUFORTH_MUFORTH_H
#define MUFORTH_MUFORTH_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


/*
 * Lexer
 */

union u_lexeme_values {
    int integer;
    double floating;char * word;
};

enum e_lexeme_type { LEXEME_TYPE_INT, LEXEME_TYPE_FLOAT, LEXEME_TYPE_WORD };

typedef struct s_lexeme {
    union u_lexeme_values value;
    enum e_lexeme_type type;
} Lexeme;

Lexeme get_lexeme(FILE* in_stream);

/* Lexer - test area */
int skip_blanks(FILE* stream);
int valid_int(char* word);


#endif //MUFORTH_MUFORTH_H
