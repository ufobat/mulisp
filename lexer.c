//
// Created by Matthieu Felix on 01/04/2016.
//

#include "muforth.h"

const int MAX_LEX = 1000;


/* Reads and discards blanks characters at the head of stream */
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


int valid_int(char* word)
{
    if(*word == '-')
        word++;

    if(*word == '\0')
        return 0;

    while(*word) {
        if(!isdigit(*word))
            return 0;
        word++;
    }

    return 1;
}


Lexeme get_lexeme(FILE* in_stream)
{
    Lexeme lexeme;
    int c, len = 0;
    char* word_val = malloc((MAX_LEX+1)*sizeof(char));

    skip_blanks(in_stream);

    for(;;) {
        c = getc(in_stream);
        if(c == EOF || isspace(c))
            break;

        if(len > MAX_LEX) {
            fprintf(stderr, "Warning: token starting with %10s... is too large. Only the first %d chars will be taken "
                    "into account", word_val, MAX_LEX);
            break;
        }

        word_val[len++] = (char)c;
    }

    // TODO: add support for floats
    if(valid_int(word_val)) {
        lexeme.type = LEXEME_TYPE_INT;
        lexeme.value.integer = atoi(word_val);
        free(word_val);
    }
    else {
        lexeme.type = LEXEME_TYPE_WORD;
        lexeme.value.word = word_val;
    }

    return lexeme;
}