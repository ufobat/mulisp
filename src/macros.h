/**
 * macros.h: various helper macros
 */

#ifndef MULISP_MACROS_H
#define MULISP_MACROS_H

#define CAR(obj) (obj->pair.car)
#define CDR(obj) (obj->pair.cdr)
#define CAAR(obj) (obj->pair.car->pair.car)
#define CADR(obj) (obj->pair.car->pair.cdr)
#define CDAR(obj) (obj->pair.cdr->pair.car)

#endif //MULISP_MACROS_H
