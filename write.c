#include <stdio.h>
#include "mulisp.h"

void write_item(Object *object);

void write_inside_list(Object *object)
{
    if(object->pair.cdr->type == OTYPE_NIL) {
        write_item(object->pair.car);
        printf(")");
    }
    else if(object->pair.cdr->type == OTYPE_PAIR) {
        write_item(object->pair.car);
        printf(" ");
        write_inside_list(object->pair.cdr);
    }
    else {
        write_item(object->pair.car);
        printf(" . ");
        write_item(object->pair.cdr);
        printf(")");
    }
}


void write_item(Object *object)
{
    int i;

    switch (object->type) {
        case OTYPE_NIL:
            printf("()");
            break;
        case OTYPE_INT:
            printf("%d", object->integer.value);
            break;
        case OTYPE_FLT:
            printf("%lf", object->floating.value);
            break;
        case OTYPE_FRAC:
            printf("%d/%u", object->fraction.numerator, object->fraction.denominator);
            break;
        case OTYPE_CHR:
            printf("#\\%c", object->chr.value);
            break;
        case OTYPE_BOOL:
            printf("#%s", object->boolean.value ? "t" : "f");
            break;
        case OTYPE_STR:
            printf("\"%s\"", object->str.value);
            break;
        case OTYPE_VECTOR:
            printf("#(");
            for(i = 0; i < object->vector.length; i++) {
                write_item(object->vector.value[i]);
                if(i < object->vector.length - 1)
                    printf(" ");
            }
            printf(")");
            break;
        case OTYPE_PAIR:
            printf("(");
            write_inside_list(object);
            break;
        case OTYPE_PROC:
            printf("<procedure>");
            break;
        case OTYPE_PORT:
            printf("<port>");
            break;
    }
}

void write(Object *object)
{
    write_item(object);
    printf("\n");
}
