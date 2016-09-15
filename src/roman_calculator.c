/* roman_calculator.c */

#include <stdlib.h>
#include <string.h>

static const char roman_characters[7] = {'I', 'V', 'X', 'L', 'C', 'D', 'M'};
typedef enum {RCI_I, RCI_V, RCI_X, RCI_L, RCI_C, RCI_D, RCI_M, RCI_END} roman_character_index;

static void count_occurrences_of_chars_IVXLCDM(const char *roman_numeral, int **symbol_counts_ptr);
static void compute_carryovers(int **symbol_counts_ptr);
static int value_of_next_numeral_in_terms_of_numeral_at(roman_character_index current_index);
static char *character_counts_to_string(const int *character_counts);
static int requires_subtractive_notation(roman_character_index index, int character_repetitions);
static void insert_subtractive_form(char **location, roman_character_index index, int quantity);
static void insert_copies_of_character(char **location, roman_character_index index_of_character, int number_of_copies);
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

/** Replaces multiple copies of a Roman digit with the next largest one. */
static void compute_carryovers(int **character_counts_ptr) {
    roman_character_index index;

    int conversion_rate;
    int quotient;

    for (index = RCI_I; index < RCI_END; index++) {
        conversion_rate = value_of_next_numeral_in_terms_of_numeral_at(index);

        quotient = (*character_counts_ptr)[index] / conversion_rate;
        (*character_counts_ptr)[index] = (*character_counts_ptr)[index] % conversion_rate;

        (*character_counts_ptr)[index + 1] += quotient;
    }
}

static int value_of_next_numeral_in_terms_of_numeral_at(roman_character_index current_index) {
    /* Five copies of I, X, or C should be converted to a single V, L, or D,
     * while two copies of V, L, or D should be converted to X, C, or M. */
    return (current_index % 2 == 0) ? 5 : 2;
}

/**
 * Indicate a subtractive form is needed to print the Roman numeral with the
 * passed character counts. This is accomplished by replacing character counts
 * indicating one 'D' followed by four 'C' chars with zero 'D' followed by nine
 * 'C' (similar for 'L' and 'X' as well as 'V' and 'I').
 */
static void flag_where_subtractive_forms_are_needed(int **character_counts_ptr) {
    roman_character_index index;
    for (index = RCI_I; index < RCI_END; index += 2) {
        if ((*character_counts_ptr)[index + 1] == 1 && (*character_counts_ptr)[index] == 4) {
            (*character_counts_ptr)[index] = 9;
            (*character_counts_ptr)[index + 1] = 0;
        }
    }
}

static char *character_counts_to_string(const int *character_counts) {

    char *result = calloc(array_sum(character_counts) + 1, sizeof(char));
    char *current_position = result;

    int current_character_count;

    roman_character_index index = RCI_M;
    for (index = RCI_M; index < RCI_END; index--) {
        current_character_count = character_counts[index];

        if (requires_subtractive_notation(index, current_character_count)) {
            insert_subtractive_form(&current_position, index, current_character_count);
        } else {
            insert_copies_of_character(&current_position, index, current_character_count);
        }
    }

    return result;
}

static int requires_subtractive_notation(roman_character_index index, int character_repetitions)
{
    return index != RCI_M && character_repetitions >= 4;
}

static void insert_subtractive_form(char **location, roman_character_index index, int quantity)
{
    roman_character_index index_of_larger_character = index + 1;
    if (quantity > 4) {
        index_of_larger_character++;
    }

    (*location)[0] = roman_characters[index];
    (*location)[1] = roman_characters[index_of_larger_character];

    *location += 2;
}

static void insert_copies_of_character(char **location, roman_character_index index_of_character, int number_of_copies)
{
    memset(*location, roman_characters[index_of_character], number_of_copies);
    *location += number_of_copies;
}

static int array_sum(const int *array) {
    int sum = 0;
    int index;
    for (index = 0; index < 7; index++) {
        sum += array[index];
    }
    return sum;
}
