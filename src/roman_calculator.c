/* roman_calculator.c */

#include <stdlib.h>
#include <string.h>

static const char roman_characters[7] = {'I', 'V', 'X', 'L', 'C', 'D', 'M'};
typedef enum {RCI_I, RCI_V, RCI_X, RCI_L, RCI_C, RCI_D, RCI_M, RCI_END} roman_character_index;

static void count_occurrences_of_chars_IVXLCDM(const char *roman_numeral, int **symbol_counts_ptr);
static void compute_carryovers(int **symbol_counts_ptr);
static char *character_counts_to_string(const int *character_counts);
static void flag_where_subtractive_forms_are_needed(int **character_counts_ptr);
static int array_sum(const int *array);

char *add_roman_numerals(const char *summand1, const char *summand2)
{
    int *character_counts = calloc(7, sizeof(size_t));

    count_occurrences_of_chars_IVXLCDM(summand1, &character_counts);
    count_occurrences_of_chars_IVXLCDM(summand2, &character_counts);

    compute_carryovers(&character_counts);
    flag_where_subtractive_forms_are_needed(&character_counts);

    char *sum = character_counts_to_string(character_counts);

    free(character_counts);
    return sum;
}

/**
 * Count occurrences of the characters I, V, X, L, C, D, and M in the input and
 * add them to the array pointed to by digit_counts_ptr.
 */
static void count_occurrences_of_chars_IVXLCDM(const char *roman_numeral,
                                               int **character_counts_ptr) {
    int offset;
    roman_character_index index;
    for (offset = 0; offset < strlen(roman_numeral); offset++) {
        for (index = RCI_I; index < RCI_END; index++) {
            if (roman_numeral[offset] == roman_characters[index]) {
                (*character_counts_ptr)[index]++;
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
}

/**
 * Indicate a subtractive form is needed to print the Roman numeral with the
 * passed character counts. This is accomplished by replacing character counts
 * indicating one 'D' followed by four 'C' chars with zero 'D' followed by nine
 * 'C' (similar for 'L' and 'X' as well as 'V' and 'I').
 */
static void flag_where_subtractive_forms_are_needed(int **character_counts_ptr) {
    int i;
    for (i = 0; i < 6; i += 2) {
        if ((*character_counts_ptr)[i + 1] == 1 && (*character_counts_ptr)[i] == 4) {
            (*character_counts_ptr)[i] = 9;
            (*character_counts_ptr)[i + 1] = 0;
        }
    }
}

/**
 * Creates a string representation of a Roman numeral with the number of
 * repetitions of the Roman digits appearing in the string specified by the
 * passed character_counts array.
 */
static char *character_counts_to_string(const int *character_counts) {

    char *roman_numeral = calloc(array_sum(character_counts) + 1, sizeof(char));

    char *current_position = roman_numeral;
    int current_character_count;

    int i;
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

static int array_sum(const int *array) {
    int sum = 0;
    int index;
    for (index = 0; index < 7; index++) {
        sum += array[index];
    }
    return sum;
}
