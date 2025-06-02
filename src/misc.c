#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <zos_vfs.h>
#include <zos_sys.h>

#include "mulisp.h"

char BUFFER[1024];

void fatal_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    uint16_t size;

    // fprintf(stdout, "***");
    sprintf(BUFFER, "***");
    size = strlen(BUFFER);
    write(DEV_STDOUT, BUFFER, &size);

    // vfprintf(stdout, fmt, args);
    vsprintf(BUFFER, fmt, args);
    size = strlen(BUFFER);
    write(DEV_STDOUT, BUFFER, &size);

    // fflush(stdout);
    // fflush(stderr);
    // exit(EXIT_FAILURE);
    exit(ERR_FAILURE);

    va_end(args);
}

bool is_atom(Object *object)
{
    return object->type != OTYPE_PAIR;
}

List *append_list(List *l1, List *l2)
{
    List *last_l1;

    if (l1 == NULL)
        return l2;

    for (last_l1 = l1; last_l1->next != NULL; last_l1 = last_l1->next);

    last_l1->next = l2;

    return l1;
}