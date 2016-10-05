/**
 * \file init.c
 * Initialize the interpreter
 */

#include "mulisp.h"
#include "prim.h"

Environment *global_environment;
Object *nil;
Object *f;
Object *t;


/**
 * make_prim create a new Object encapsulating a primitive, and registers in
 * in the global environment.
 * @param identifier The name to give to the primitive
 * @param f A function pointer to the wrapped primitive
 * @return The newly-registered object
 */
Object *make_prim(char *identifier, Object *(*f)(Object *, Environment *))
{
    Object *ret = malloc(sizeof(Object));
    ret->type = OTYPE_PRIM;
    ret->prim.f = f;
    define_object(identifier, ret, global_environment);

    return ret;
}


/**
 * init creates the global environment, the values of `nil`, `t` and `f`, and
 * adds primitives to it.
 */
void init()
{
    global_environment = new_env(NULL);

    nil = malloc(sizeof(Object));
    nil->type = OTYPE_NIL;

    t = malloc(sizeof(Object));
    t->type = OTYPE_BOOL;
    t->boolean.value = 1;

    f = malloc(sizeof(Object));
    f->type = OTYPE_BOOL;
    f->boolean.value = 0;

    make_prim("+", prim_plus);
    make_prim(">", prim_gt);
    make_prim("-", prim_minus);
    make_prim("=", prim_equal);
    make_prim("*", prim_multiply);
}