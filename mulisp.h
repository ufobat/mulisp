//
// Created by Matthieu Felix on 01/04/2016.
//

#include <stdio.h>
#include <stdlib.h>

#ifndef MUFORTH_MUFORTH_H
#define MUFORTH_MUFORTH_H

#define MAX(a, b) (a > b ? a : b)

/*
 * General structures
 */

typedef struct s_list {
    struct s_list * next;
    void * item;
} List;

/*
 * Objects
 */

typedef struct s_object {
    enum { OTYPE_INT, OTYPE_FLT, OTYPE_FRAC,
           OTYPE_PAIR, OTYPE_BOOL, OTYPE_PROC } type;
    union {
        struct {
            int value;
        } integer;
        struct {
            double value;
        } floating;
        struct {
            int denumerator;
            unsigned numerator;
        } fraction;
        struct {
            char value;
        } boolean;
        struct {
            struct s_object * car;
            struct s_object * cdr;
        } pair;
    };
} Object;


/*
 * Tokenizer - tokenize.c
 */

List* tokenize(char* string);


/*
 * Lists - list.c
 */


/*
 * misc.c
 */

void fatal_error(const char * fmt, ...);

#endif //MUFORTH_MUFORTH_H
