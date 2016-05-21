#include <stdio.h>
#include <stdlib.h>
#include "mulisp.h"


void tokenize_tester(char * str) {
    List * ans = tokenize(str);

    while(ans != NULL) {
        printf(" %s |", (char*)ans->item);
        ans = ans->next;
    }

    printf("\n");
}


int main()
{
    tokenize_tester("hello");
    tokenize_tester(" (abc    \"def ghi ::-()\" 3)')   ");

    return 0;
}
