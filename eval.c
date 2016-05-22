#include "mulisp.h"

Object *eval(Object *to_eval, Environment *env) {
    Object *ret;

    switch(to_eval->type) {
        case OTYPE_STR:
        case OTYPE_NIL:
        case OTYPE_BOOL:
        case OTYPE_FLT:
        case OTYPE_INT:
        case OTYPE_FRAC:
        case OTYPE_CHR:
            return to_eval;

        default:
            ret = make_symbol("Not implemented");
    }

    return ret;
}