/* roman_calculator.c */

#include <stdlib.h>
#include <string.h>

char *add_roman_numerals(const char *summand1, const char *summand2)
{
    char *sum = calloc(strlen(summand1) + strlen(summand2) + 1, sizeof(char));
    strcat(sum, summand1);
    strcat(sum, summand2);

    return sum;
}
