#include <stdio.h>
#include <stdlib.h>
#include "mulisp.h"

void test_parser(char* str)
{
    List * lst = tokenize(str);

    printf("%s\n", (char*)lst->item);

    Object * res = parse(&lst, 0);

    printf("Type %d\n", res->type);
}

void test_number_parser(char* str) {
    int integer;
    double floating;
    int numerator;
    unsigned denominator;

    printf("%s -> ", str);

    if(parse_int(str, &integer))
        printf("Integer, %d\n", integer);
    else if(parse_float(str, &floating))
        printf("Float, %lf\n", floating);
    else if(parse_frac(str, &numerator, &denominator))
        printf("Fraction, %d/%u\n", numerator, denominator);
    else
        printf("No match\n");
}

int main()
{
    printf("Number parsing --\n");
    test_number_parser("12");
    test_number_parser("12.");
    test_number_parser("0");
    test_number_parser("abc");
    test_number_parser("-");
    test_number_parser("-.");
    test_number_parser(".");
    test_number_parser("-24");
    test_number_parser("-.24");
    test_number_parser("-6.345");
    test_number_parser("-/");
    test_number_parser("/0");
    test_number_parser("1/");
    test_number_parser("2/3");
    test_number_parser("-18/89");

    printf("General parsing --\n");
    test_parser("\"hello\"");
    test_parser("#t");
    test_parser("#f");
    test_parser("#\\c");
    test_parser("21");

    return 0;
}
