//
// Created by Matthieu Felix on 01/10/2016.
//

#include "mulisp.h"
#include <stdbool.h>

#define MAX_OBJ 4096

#define GC_MARK 0x01
#define GC_ENV 0x02

/* objects_array contains both objects and environments. Environments are
 * flagged with GC_ENV. */
void *objects_array[MAX_OBJ];
short objects_flags[MAX_OBJ];


/* Finds a free index in the objects array, or -1 if there is none. */
static int find_free_index(void)
{
    for (int i = 0; i < MAX_OBJ; i++) {
        if (objects_array[i] == NULL)
            return i;
    }

    return -1;
}


void init_gc(void)
{
    for (int i = 0; i < MAX_OBJ; i++) {
        objects_array[i] = NULL;
        objects_flags[i] = 0;
    }
}


Object *new_object(enum e_type type)
{
    int free_index = find_free_index();
    if (free_index == -1)
        fatal_error("No more space for new objects");

    Object *ret = malloc(sizeof(Object));
    ret->type = type;
    objects_array[free_index] = ret;

    return ret;
}


void collect_garbage(Environment *environment)
{
    (void)environment;
}