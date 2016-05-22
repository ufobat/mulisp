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

enum e_type {OTYPE_INT, OTYPE_FLT, OTYPE_FRAC, OTYPE_CHR, OTYPE_VECTOR, OTYPE_STR, OTYPE_PORT,
    OTYPE_PAIR, OTYPE_BOOL, OTYPE_PROC, OTYPE_NIL};

typedef struct s_object {
    enum e_type type;
    union {
        struct {
            int value;
        } integer;
        struct {
            double value;
        } floating;
        struct {
            unsigned denominator;
            int numerator;
        } fraction;
        struct {
            int value;
        } chr;
        struct {
            int length;
            struct s_object ** value;
        } vector;
        struct {
            int length;
            char * value;
        } str;
        struct {
            int flags;
            void * accessed;
        } port;
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
            // add env
        } proc;
    };
} Object;


/*
 * Tokenizer - tokenize.c
 */

List* tokenize(char* string);


/*
 * Parser - parse.c
 */

Object* parse(List** tokens_pointer);


/*
 * Writer - write.c
 */

void write(Object *object);

/*
 * misc.c
 */

void fatal_error(const char * fmt, ...);

/*
 * For testing - remove afterwards.
 */

int parse_int(char *tok, int *value);
int parse_float(char *tok, double *value);
int parse_frac(char *tok, int *num, unsigned *denom);


/*
 * Constants
 */

extern Object nil;
extern Object quote;
extern Object t;
extern Object f;

#endif //MUFORTH_MUFORTH_H
