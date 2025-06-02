#include <stdarg.h>
#include "mulisp.h"
#include "zos_sys.h"

void fatal_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    printf("*** ");
    vprintf(fmt, args);
    printf("\n");
    exit(1);

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