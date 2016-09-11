/* roman_calculator.c */

#include <stdlib.h>
#include <string.h>

static const char roman_characters[7] = {'I', 'V', 'X', 'L', 'C', 'D', 'M'};
static void count_occurrences_of_chars_IVXLCDM(const char *roman_numeral,
                                               int **symbol_counts_ptr);

char *add_roman_numerals(const char *summand1, const char *summand2)
{
    int *character_counts = calloc(7, sizeof(size_t));

    count_occurrences_of_chars_IVXLCDM(summand1, &character_counts);
    count_occurrences_of_chars_IVXLCDM(summand2, &character_counts);

    /* Compute carry-overs */
    int i = 0;
    int next_symbol_conversion = 0;
    int remainder = 0;
    int quotient = 0;
    for (i = 0; i < 6; i++) {
        next_symbol_conversion = (i % 2 == 0) ? 5 : 2;
        remainder = character_counts[i] % next_symbol_conversion;
        quotient = character_counts[i] / next_symbol_conversion;
        character_counts[i] = remainder;
        character_counts[i + 1] += quotient;
    }

    char *sum = calloc(strlen(summand1) + strlen(summand2) + 1, sizeof(char));

    int offset = 0;
    for (i = 6; i > -1; i--) {
        memset(sum + offset, roman_characters[i], character_counts[i]);
        offset += character_counts[i];
    }

    free(character_counts);
    return sum;
}

/**
 * Count occurrences of the characters I, V, X, L, C, D, and M in the
 * input and add them to the array pointed to by digit_counts_ptr.
 */
static void count_occurrences_of_chars_IVXLCDM(const char *roman_numeral,
                                               int **character_counts_ptr) {
    int i, j;
    for (i = 0; i < strlen(roman_numeral); i++) {
        for (j = 0; j < 7; j++) {
            if (roman_numeral[i] == roman_characters[j]) {
                (*character_counts_ptr)[j]++;
            }
        }
    }
}
