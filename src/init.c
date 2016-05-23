#include "mulisp.h"

Environment *global_environment;
Object *nil;
Object *f;
Object *t;
Object *quote;
Object *quasiquote;
Object *unquote;

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

    quote = make_symbol("quote");
    define_object("quote", quote, global_environment);

    quasiquote = make_symbol("quasiquote");
    define_object("quasiquote", quasiquote, global_environment);

    unquote = make_symbol("unquote");
    define_object("unquote", unquote, global_environment);
}