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
    enum { OTYPE_INT, OTYPE_FLT, OTYPE_FRAC, OTYPE_CHR, OTYPE_VECTOR, OTYPE_STR, OTYPE_PORT,
           OTYPE_PAIR, OTYPE_BOOL, OTYPE_PROC, OTYPE_NIL } type;
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
            int value;
        } chr;
        struct {
            int length;
            struct s_object * value;
        } vector;
        struct {
            int length;
            int * value;
        } str;
        struct {
            char value;
        } boolean;
        struct {
            struct s_object * car;
            struct s_object * cdr;
        } pair;
        struct {
            struct s_object * arguments;
            struct s_object * body;
        } proc;
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
