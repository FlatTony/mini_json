#ifndef CTEST_H_INCLUDED
#define CTEST_H_INCLUDED

void assertEQ_s(char *name, char *str1, char *str2);
void assertEQ_d(char *name, int in1, int in2);
void assertEQ_l(char *name, long int in1, long int in2);
void assertEQ_f(char *name, double d1, double d2);

#endif // CTEST_H_INCLUDED
