#include "mulisp.h"

void test_parser(char* str)
{
    List * lst = tokenize(str);

    Object * res = parse(&lst);

    write(res);
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

void test_env()
{
    printf("%u == %u ?\n", get_hash("A`"), get_hash("BA"));

    /* Collision checking */
    define_object("A`", NULL, global_environment);
    define_object("BA", make_symbol("hello"), global_environment);
    write(get_object("BA", global_environment));
    set_object("A`", make_symbol("This is A`"), global_environment);
    write(get_object("A`", global_environment));

    /* Parent envs */
    Environment *son_env = new_env(global_environment);
    define_object("x", make_symbol("This is x"), son_env);
    set_object("A`", make_symbol("This is A` in parent"), global_environment);
    define_object("A`", make_symbol("This is A` in son"), son_env);
    write(get_object("x", son_env));
    write(get_object("A`", son_env));
    write(get_object("A`", global_environment));
    write(get_object("BA", son_env));

    delete_env(son_env);
}

void run_tests()
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

    printf("General object parsing --\n");
    test_parser("\"hello\"");
    test_parser("#t");
    test_parser("#f");
    test_parser("#\\c");
    test_parser("21");
    test_parser("()");
    test_parser("(1 . 2)");
//    test_parser("(1 . 2");
    test_parser("(1 2 . 3)");
    test_parser("(1)");
    test_parser("(1 \"hello\")");
    test_parser("((1 2) 3 . ())");
    test_parser("abc");

    test_env();
}