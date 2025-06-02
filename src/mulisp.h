//
// Created by Matthieu Felix on 01/04/2016.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef MUFORTH_MUFORTH_H
#define MUFORTH_MUFORTH_H

#define VERSION_STRING "2025-06-02"

#define HASHTBL_SIZE 10

#include "macros.h"

#define QUOTE_STRING "quote"

/*
 * General structures
 */

typedef struct s_list
{
    struct s_list *next;
    void *item;
} List;


struct s_hashtable_entry
{
    struct s_hashtable_entry *next;
    char *key;
    struct s_object *refer;
};

typedef struct s_environment
{
    struct s_environment *parent;
    struct s_hashtable_entry *bindings[HASHTBL_SIZE];
} Environment;

/*
 * Objects
 */

enum e_type
{
    OTYPE_INT, OTYPE_FLT, OTYPE_FRAC, OTYPE_CHR, OTYPE_VECTOR, OTYPE_STR, OTYPE_PORT,
    OTYPE_PAIR, OTYPE_BOOL, OTYPE_PROC, OTYPE_PRIM, OTYPE_NIL, OTYPE_SYM
};

typedef struct s_object
{
    enum e_type type;
    union
    {
        struct
        {
            int value;
        } integer;
        struct
        {
            float value;
        } floating;
        struct
        {
            unsigned denominator;
            int numerator;
        } fraction;
        struct
        {
            int value;
        } chr;
        struct
        {
            int length;
            struct s_object **value;
        } vector;
        struct
        {
            int length;
            char *value;
        } str;
        struct
        {
            int flags;
            void *accessed;
        } port;
        struct
        {
            char value;
        } boolean;
        struct
        {
            struct s_object *car;
            struct s_object *cdr;
        } pair;
        struct
        {
            struct s_object *arguments;
            struct s_object *body;
            Environment *env;
        } proc;

        struct
        {
            struct s_object *(*f)(struct s_object *args, Environment *env);
        } prim;
    };
} Object;


/*
 * Initialization - init.c
 */

void init(void);

/*
 * Tokenizer - tokenize.c
 */

List *tokenize(char *string);


/*
 * Parser - parse.c
 */

Object *make_symbol(char *val);

Object *parse_and_free(List **tokens_pointer);

/*
 * Writer - write.c
 */

void write_newline(Object *object);

void write_item(Object *object);

/*
 * Evaluator - eval.c
 */

Object *eval(Object *to_eval, Environment *env);

/*
 * Environments - env.c
 */

struct s_hashtable_entry *get_entry(char *identifier, Environment *env);

Object *get_object(char *identifier, Environment *env);

void define_object(char *identifier, Object *obj, Environment *env);

void set_object(char *identifier, Object *obj, Environment *env);

Environment *new_env(Environment *parent);

void delete_env(Environment *env);

/*
 * misc.c
 */

void fatal_error(const char *fmt, ...);

List *append_list(List *l1, List *l2);

/*
 * For testing - remove afterwards.
 */

int parse_int(char *tok, int *value);

// int parse_float(char *tok, float *value);

int parse_frac(char *tok, int *num, unsigned *denom);

unsigned get_hash(char *str);

void run_tests(void);

bool is_atom(Object *object);


/*
 * Global objects
 */

extern Object *nil;
extern Object *t;
extern Object *f;
extern Environment *global_environment;

#endif //MUFORTH_MUFORTH_H
