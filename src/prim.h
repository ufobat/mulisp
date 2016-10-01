#ifndef MULISP_PRIM_H
#define MULISP_PRIM_H

#include "mulisp.h"

Object *plus(Object *params, Environment *env);

Object *gt(Object *params, Environment *env);

Object *minus(Object *params, Environment *env);

Object *arithmetic_equal(Object *params, Environment *env);

Object *multiply(Object *params, Environment *env);

#endif //MULISP_PRIM_H
