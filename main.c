#include "muforth.h"

void test_ints();

void main()
{
    test_ints();
}

void test_ints()
{
    const int NB_TESTS = 8;
    char* strings[NB_TESTS] = {"", "-", "1", "-1", "67.89", "899", "8ab", "a89"};

    printf("valid_int tests\n");

    for(int i = 0; i < NB_TESTS; i++) {
        printf("%s: %d\n", strings[i], valid_int(strings[i]));
    }
}