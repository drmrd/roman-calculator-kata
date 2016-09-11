/* roman_calculator.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *add_roman_numerals(const char *summand1, const char *summand2)
{
    /*
     * Count up the number of the symbols I, V, X, L, C, D, and M in
     * summand1 and summand2.
     */
    char symbols[7] = {'I', 'V', 'X', 'L', 'C', 'D', 'M'};
    int symbol_counts[7] = {0};
    int i = 0;
    int j = 0;

    for (i = 0; i < 7; i++) {
        for (j = 0; j < strlen(summand1); j++) {
            if (summand1[j] == symbols[i]) symbol_counts[i]++;
        }
    }

    for (i = 0; i < 7; i++) {
        for (j = 0; j < strlen(summand2); j++) {
            if (summand2[j] == symbols[i]) symbol_counts[i]++;
        }
    }

    /* Compute carry-overs */
    int next_symbol_conversion = 0;
    int remainder = 0;
    int quotient = 0;
    for (i = 0; i < 6; i++) {
        next_symbol_conversion = (i % 2 == 0) ? 5 : 2;
        remainder = symbol_counts[i] % next_symbol_conversion;
        quotient = symbol_counts[i] / next_symbol_conversion;
        symbol_counts[i] = remainder;
        symbol_counts[i + 1] += quotient;
    }

    char *sum = calloc(strlen(summand1) + strlen(summand2) + 1, sizeof(char));

    int offset = 0;
    for (i = 6; i > -1; i--) {
        memset(sum + offset, symbols[i], symbol_counts[i]);
        offset += symbol_counts[i];
    }

//    strcat(sum, summand1);
//    strcat(sum, summand2);

    return sum;
}
