/* roman_calculator.c */

#include <stdlib.h>
#include <string.h>

static const char roman_characters[7] = {'I', 'V', 'X', 'L', 'C', 'D', 'M'};
typedef enum {RCI_I, RCI_V, RCI_X, RCI_L, RCI_C, RCI_D, RCI_M, RCI_END} roman_character_index;

static int not_a_roman_numeral(const char *input);
static void count_occurrences_of_roman_characters(const char *roman_numeral, int **symbol_counts_ptr);
static void remove_negative_character_counts_by_borrowing(int **character_counts);
static int relative_roman_character_value(char old_char, char new_char);
static int is_a_subtractive_form(char first_character, char second_character);
static void compute_carryovers(int **symbol_counts_ptr);
static int value_of_next_numeral_in_terms_of_numeral_at(roman_character_index current_index);
static char *character_counts_to_string(const int *character_counts);
static int requires_subtractive_notation(roman_character_index index, int character_repetitions);
static void insert_subtractive_form(char **location, roman_character_index index, int quantity);
static void insert_copies_of_character(char **location, roman_character_index index_of_character, int number_of_copies);
static void flag_where_subtractive_forms_are_needed(int **character_counts_ptr);
static roman_character_index get_index(char roman_character);
static int sum_over_array(const int *array);
static void subtract_arrays(int **array1, int **array2);

char *add_roman_numerals(const char *summand1, const char *summand2)
{
    if (not_a_roman_numeral(summand1) || not_a_roman_numeral(summand2)) {
         exit(EXIT_FAILURE);
    }

    int *character_counts = calloc(7, sizeof(int));

    count_occurrences_of_roman_characters(summand1, &character_counts);
    count_occurrences_of_roman_characters(summand2, &character_counts);

    compute_carryovers(&character_counts);
    flag_where_subtractive_forms_are_needed(&character_counts);

    char *sum = character_counts_to_string(character_counts);

    free(character_counts);
    return sum;
}

char *subtract_roman_numerals(const char *numeral1, const char *numeral2)
{
    int *character_counts = calloc(7, sizeof(int));
    int *numeral2_counts = calloc(7, sizeof(int));

    count_occurrences_of_roman_characters(numeral1, &character_counts);
    count_occurrences_of_roman_characters(numeral2, &numeral2_counts);

    subtract_arrays(&character_counts, &numeral2_counts);
    remove_negative_character_counts_by_borrowing(&character_counts);

    compute_carryovers(&character_counts);
    flag_where_subtractive_forms_are_needed(&character_counts);

    char *difference = character_counts_to_string(character_counts);

    free(numeral2_counts);
    free(character_counts);
    return difference;
}

static void remove_negative_character_counts_by_borrowing(int **character_counts) {
    int current_count;
    roman_character_index current, nearest_positive;

    for (current = RCI_M; current < RCI_END; current--) {
        current_count = (*character_counts)[current];

        if (current_count > 0) {
            nearest_positive = current;
        } else if (current_count < 0) {
            (*character_counts)[current] += relative_roman_character_value(
                                                roman_characters[nearest_positive],
                                                roman_characters[current]
                                            );
            (*character_counts)[nearest_positive]--;
            nearest_positive = current;
        }
    }
}

static int not_a_roman_numeral(const char *input) {
    size_t input_length = strlen(input);
    if (input_length == 0) return 1;

    size_t current_index;
    for(current_index = 0; current_index < input_length; current_index++) {
        if (get_index(input[current_index]) == RCI_END) return 1;
    }
    return 0;
}

/**
 * Count occurrences of the characters I, V, X, L, C, D, and M in the input and
 * add them to the array pointed to by character_counts_ptr.
 */
static void count_occurrences_of_roman_characters(const char *roman_numeral,
                                                  int **character_counts_ptr) {
    char current_char;
    char next_char = *roman_numeral;
    size_t offset;
    size_t copies_of_current_char;
    for (offset = 0; offset < strlen(roman_numeral); offset++) {
        copies_of_current_char = 1;

        current_char = next_char;
        next_char = roman_numeral[offset + 1];

        if (next_char != '\0' && is_a_subtractive_form(current_char, next_char)) {
            copies_of_current_char = relative_roman_character_value(next_char, current_char) - 1;
            offset++;
        }
        (*character_counts_ptr)[get_index(current_char)] += copies_of_current_char;
    }
}

static int relative_roman_character_value(char old_char, char new_char) {
    const int relative_value_of_character_after_new_char = (get_index(new_char) % 2 == 0) ? 5 : 2;
    switch (get_index(old_char) - get_index(new_char)) {
        case 1: return relative_value_of_character_after_new_char;
        case 2: return 10;
        case 3: return relative_value_of_character_after_new_char * 10;
        case 4: return 100;
        case 5: return relative_value_of_character_after_new_char * 100;
        case 6: return 1000;
        default: return -1;
    }
}

static int is_a_subtractive_form(char first_character, char second_character) {
    return get_index(first_character) < get_index(second_character);
}

static roman_character_index get_index(char roman_character) {
    roman_character_index index;

    for (index = RCI_I; index < RCI_END; index++) {
         if (roman_character == roman_characters[index]) break;
    }
    return index;
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

    char *result = calloc(sum_over_array(character_counts) + 1, sizeof(char));
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

static int sum_over_array(const int *array) {
    int sum = 0;
    int index;
    for (index = 0; index < 7; index++) {
        sum += array[index];
    }
    return sum;
}

static void subtract_arrays(int **array1, int **array2) {
    size_t index;
    for (index = 0; index < 7; index++) {
        (*array1)[index] -= (*array2)[index];
    }
}
