#include <stdio.h>
#include <stdlib.h>
#include "mulisp.h"

int main()
{
    char * tok;

    do {
        tok = get_next_token(stdin);
        if(tok) {
            printf("%s\n", tok);
            free(tok);
        }
    } while(tok != NULL);

    return 0;
}
