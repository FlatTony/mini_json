#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void assertEQ_s(char *name, char *str1, char *str2)
{
    char result[15];
    strcpy(result, "failed");

    printf("Testing: %s, ", name);

    if (strcmp(str1,str2) == 0)
    {
        strcpy(result, "passed");
        printf("Result: %s\n", result);
    }
    else
    {
        printf("Result: %s ", result);
        printf("Got: %s, Expected: %s\n", str1, str2);
    }
}
void assertEQ_d(char *name, int in1, int in2)
{
    char result[15];
    strcpy(result, "failed");

    printf("Testing: %s, ", name);

    if (in1 == in2)
    {
        strcpy(result, "passed");
        printf("Result: %s\n", result);
    }
    else
    {
        printf("Result: %s ", result);
        printf("Got: %d, Expected: %d\n", in1, in2);
    }
}

void assertEQ_l(char *name, long int in1, long int in2)
{
    char result[15];
    strcpy(result, "failed");

    printf("Testing: %s, ", name);

    if (in1 == in2)
    {
        strcpy(result, "passed");
        printf("Result: %s\n", result);
    }
    else
    {
        printf("Result: %s ", result);
        printf("Got: %ld, Expected: %ld\n", in1, in2);
    }
}
void assertEQ_f(char *name, double d1, double d2)
{
    char result[15];
    strcpy(result, "failed");

    printf("Testing: %s, ", name);

    if (d1 == d2)
    {
        strcpy(result, "passed");
        printf("Result: %s\n", result);
    }
    else
    {
        printf("Result: %s ", result);
        printf("Got: %f, Expected: %f\n", d1, d2);
    }
}
