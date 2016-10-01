#include "mulisp.h"
#include "prim.h"

Environment *global_environment;
Object *nil;
Object *f;
Object *t;

Object *make_prim(char *identifier, Object *(*f)(Object *args, Environment *env))
{
    Object *ret = malloc(sizeof(Object));
    ret->type = OTYPE_PRIM;
    ret->prim.f = f;
    define_object(identifier, ret, global_environment);

    return ret;
}

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

    make_prim("+", plus);
    make_prim(">", gt);
    make_prim("-", minus);
    make_prim("=", arithmetic_equal);
    make_prim("*", multiply);
}