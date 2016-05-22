#include <stdarg.h>
#include "mulisp.h"

void fatal_error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    fprintf(stdout, "***");
    vfprintf(stdout, fmt, args);
    fflush(stdout);
    fflush(stderr);
    exit(EXIT_FAILURE);

    va_end(args);
}
