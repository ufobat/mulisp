#include <stdarg.h>
#include "mulisp.h"

void fatal_error(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, fmt, args);
    exit(EXIT_FAILURE);

    va_end(args);
}
