#ifndef MULISP_PRIM_H
#define MULISP_PRIM_H

#include "mulisp.h"

Object *prim_plus(Object *params, Environment *env);

Object *prim_gt(Object *params, Environment *env);

Object *prim_minus(Object *params, Environment *env);

Object *prim_equal(Object *params, Environment *env);

Object *prim_multiply(Object *params, Environment *env);

#endif //MULISP_PRIM_H
