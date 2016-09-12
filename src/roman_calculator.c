/* roman_calculator.c */

#include <stdlib.h>
#include <string.h>

static const char roman_characters[7] = {'I', 'V', 'X', 'L', 'C', 'D', 'M'};

static void count_occurrences_of_chars_IVXLCDM(const char *roman_numeral,
                                               int **symbol_counts_ptr);
static void compute_carryovers(int **symbol_counts_ptr);
static char *character_counts_to_roman_numeral(const int *character_counts);

char *add_roman_numerals(const char *summand1, const char *summand2)
{
    int *character_counts = calloc(7, sizeof(size_t));

    count_occurrences_of_chars_IVXLCDM(summand1, &character_counts);
    count_occurrences_of_chars_IVXLCDM(summand2, &character_counts);

    compute_carryovers(&character_counts);

    char *sum = character_counts_to_roman_numeral(character_counts);

    free(character_counts);
    return sum;
}

/**
 * Count occurrences of the characters I, V, X, L, C, D, and M in the input and
 * add them to the array pointed to by digit_counts_ptr.
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

/**
 * Compute "carryovers", replacing multiple copies of a Roman digit with the
 * next largest one.
 */
static void compute_carryovers(int **character_counts_ptr) {
    /* The index of the current character in roman_characters */
    int i;
    /* The value of the next largest character in terms of the current one */
    int value_of_next_largest_char = 0;
    int quotient = 0;

    for (i = 0; i < 6; i++) {
        /* Five copies of I, X, or C should be converted to a single V, L, or D,
         * while two copies of V, L, or D should be converted to X, C, or M. */
        value_of_next_largest_char = (i % 2 == 0) ? 5 : 2;

        quotient = (*character_counts_ptr)[i] / value_of_next_largest_char;
        (*character_counts_ptr)[i] = (*character_counts_ptr)[i]
            % value_of_next_largest_char;

        (*character_counts_ptr)[i + 1] += quotient;
    }

    for (i = 0; i < 3; i++) {
        if ((*character_counts_ptr)[2 * i + 1] == 1 && (*character_counts_ptr)[2 * i] == 4) {
            (*character_counts_ptr)[2 * i] = 9;
            (*character_counts_ptr)[2 * i + 1] = 0;
        }
    }
}

/**
 * Creates a string representation of a Roman numeral with the number of
 * repetitions of the Roman digits appearing in the string specified by the
 * passed character_counts array.
 */
static char *character_counts_to_roman_numeral(const int *character_counts) {
    int numeral_length = 1;
    int i;
    for (i = 0; i < 7; i++) {
        numeral_length += character_counts[i];
    }

    char *roman_numeral = calloc(numeral_length, sizeof(char));

    char *current_position = roman_numeral;
    int current_character_count;
    for (i = 6; i > -1; i--) {
        current_character_count = character_counts[i];

        if (i == 6 || current_character_count < 4) {
            memset(current_position, roman_characters[i], current_character_count);
            current_position += current_character_count;
        } else {
            *current_position++ = roman_characters[i];
            *current_position++ = roman_characters[i + 1 + (current_character_count % 2)];
        }
    }

    return roman_numeral;
}
