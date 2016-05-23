#include "mulisp.h"

Object *plus(Object *params, Environment *env) {
    Object *ret = malloc(sizeof(Object));
    ret->type = OTYPE_INT;

    if(params->type == OTYPE_NIL)
        ret->integer.value = 0;
    else if(params->type == OTYPE_PAIR && params->pair.car->type == OTYPE_INT)
        ret->integer.value = params->pair.car->integer.value + plus(params->pair.cdr, env)->integer.value;

    return ret;
}