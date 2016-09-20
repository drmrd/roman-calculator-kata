/* roman_calculator.c */

#include <stdlib.h>
#include <string.h>

static const char roman_characters[7] = {'I', 'V', 'X', 'L', 'C', 'D', 'M'};
typedef enum {
    RCI_I, RCI_V, RCI_X, RCI_L, RCI_C, RCI_D, RCI_M, RCI_END
} rc_index;

/* Input validation */
static void  validate_input_strings_are_roman_numerals(const char *input1,
                                                       const char *input2);

/* General purpose predicate functions */
static int   at_power_of_ten(rc_index index);
static int   at_subtractive_form(rc_index index1, rc_index index2);
static int   not_a_roman_numeral(const char *input);
static int   requires_subtractive_notation(rc_index index, int count);

/* Helpers that directly manipulate a character_counts array */
static void  count_occurrences_of_roman_characters(const char *roman_numeral,
                                                   int **symbol_counts_ptr);
static void  flag_where_subtractive_forms_are_needed(int **character_counts_ptr);
static int   relative_roman_character_value(char old_char, char new_char);
static void  compute_carryovers(int **symbol_counts_ptr);
static void  borrow_to_remove_negative_character_counts(int **character_counts);
static void  replace_larger_numeral_with_smaller(int **character_counts_ptr,
                                                 rc_index larger,
                                                 rc_index smaller,
                                                 int number_to_replace);
static void  subtractive_form_to_character_counts(rc_index index1,
                                                  rc_index index2,
                                                  int **character_counts_ptr);

/* Helpers for building result strings */
static char *character_counts_to_string(const int *character_counts);
static void  insert_subtractive_form(char **location, rc_index index,
                                     int quantity);
static void  insert_copies_of_character(char **location,
                                        rc_index character_index,
                                        int number_of_copies);

/* Working with rc_index variables */
static rc_index get_index(char roman_character);
static rc_index get_index_of_first_positive_count(rc_index start,
                                                  int **character_counts_ptr);

/* Array operations */
static int   sum_over_array(const int *array);
static void  subtract_arrays(int **array1, int **array2);

char *add_roman_numerals(const char *summand1, const char *summand2)
{
    validate_input_strings_are_roman_numerals(summand1, summand2);

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
    validate_input_strings_are_roman_numerals(numeral1, numeral2);

    int *character_counts = calloc(7, sizeof(int));
    int *numeral2_counts = calloc(7, sizeof(int));

    count_occurrences_of_roman_characters(numeral1, &character_counts);
    count_occurrences_of_roman_characters(numeral2, &numeral2_counts);

    subtract_arrays(&character_counts, &numeral2_counts);
    borrow_to_remove_negative_character_counts(&character_counts);

    compute_carryovers(&character_counts);
    flag_where_subtractive_forms_are_needed(&character_counts);

    char *difference = character_counts_to_string(character_counts);

    free(numeral2_counts);
    free(character_counts);
    return difference;
}

static void validate_input_strings_are_roman_numerals(const char *input1,
                                                      const char *input2)
{
    if (not_a_roman_numeral(input1) || not_a_roman_numeral(input2)) {
        exit(EXIT_FAILURE);
    }
}

static void borrow_to_remove_negative_character_counts(int **character_counts_ptr)
{
    int *character_counts = *character_counts_ptr;
    rc_index current;
    rc_index first_positive;
    for (current = RCI_M; current < RCI_END; current--) {
        if (character_counts[current] == 0) continue;

        first_positive = get_index_of_first_positive_count(current,
                                                           &character_counts);
        if (first_positive > current) {
            replace_larger_numeral_with_smaller(&character_counts,
                                                first_positive, current, 1);
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

static void count_occurrences_of_roman_characters(const char *roman_numeral,
                                                  int **character_counts_ptr) {
    int *character_counts = *character_counts_ptr;

    rc_index current;
    rc_index next;
    size_t offset;
    for (offset = 0; offset < strlen(roman_numeral); offset++) {
        current = get_index(roman_numeral[offset]);
        next = get_index(roman_numeral[offset + 1]);

        if (at_subtractive_form(current, next)) {
            subtractive_form_to_character_counts(current, next,
                                                 &character_counts);
            offset++;
        } else {
            character_counts[current]++;
        }
    }
}

static int relative_roman_character_value(char old_char, char new_char) {
    const int relative_value_of_character_after_new_char
        = (at_power_of_ten(get_index(new_char))) ? 2 : 5;
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

static int at_power_of_ten(rc_index index) {
    return index % 2 == 1;
}

static int at_subtractive_form(rc_index first_index,
                               rc_index second_index)
{
    int difference = second_index - first_index;
    return second_index < RCI_END
        && (difference > 0)
        && (difference < 3);
}

static rc_index get_index(char roman_character) {
    rc_index index;
    for (index = RCI_I; index < RCI_END; index++) {
        if (roman_character == roman_characters[index]) break;
    }
    return index;
}

static rc_index get_index_of_first_positive_count(rc_index start,
                                                  int **character_counts_ptr)
{
    rc_index result = start + 1;
    while (result < RCI_END) {
        if ((*character_counts_ptr)[result] > 0) break;
        result++;
    }
    if (result == RCI_END) return RCI_I;
    return result;
}

/** Replaces multiple copies of a Roman digit with the next largest one. */
static void compute_carryovers(int **character_counts_ptr) {
    int *character_counts = *character_counts_ptr;

    int conversion_rate;
    int quotient;

    rc_index index;
    for (index = RCI_I; index < RCI_M; index++) {
        conversion_rate
            = relative_roman_character_value(roman_characters[index + 1],
                                             roman_characters[index]);

        quotient = character_counts[index] / conversion_rate;
        character_counts[index] = character_counts[index] % conversion_rate;

        character_counts[index + 1] += quotient;
    }
}

static void replace_larger_numeral_with_smaller(int **character_counts_ptr,
                                                rc_index larger,
                                                rc_index smaller,
                                                int number_to_replace)
{
    int *character_counts = *character_counts_ptr;
    character_counts[larger] -= number_to_replace;
    character_counts[smaller]
        += relative_roman_character_value(roman_characters[larger],
                                          roman_characters[smaller])
           * number_to_replace;
}

void subtractive_form_to_character_counts(rc_index index1, rc_index index2,
                                          int **character_counts_ptr)
{
    int *character_counts = *character_counts_ptr;

    if (!at_power_of_ten(index1) && !at_power_of_ten(index2)) {
        character_counts[index1 + 1] += 1;
        character_counts[index1] += 4;
    } else {
        character_counts[index1]
            += relative_roman_character_value(roman_characters[index2],
                                              roman_characters[index1]) - 1;
    }
}

/**
 * Indicate a subtractive form is needed to print the Roman numeral with the
 * passed character counts. This is accomplished by replacing character counts
 * indicating one 'D' followed by four 'C' chars with zero 'D' followed by nine
 * 'C' (similar for 'L' and 'X' as well as 'V' and 'I').
 */
static void flag_where_subtractive_forms_are_needed(int **character_counts_ptr) {
    rc_index current;
    int *character_counts = *character_counts_ptr;

    /* V, L, & D are never the subtracted part of a form, so we skip them. */
    for (current = RCI_I; current < RCI_END; current += 2) {
        if (character_counts[current] == 4) {
            replace_larger_numeral_with_smaller(&character_counts,
                                                current + 1, current,
                                                character_counts[current + 1]);
        }
    }
}

static char *character_counts_to_string(const int *character_counts) {

    char *result = calloc(sum_over_array(character_counts) + 1, sizeof(char));
    char *current_position = result;

    int current_character_count;

    rc_index index = RCI_M;
    for (index = RCI_M; index < RCI_END; index--) {
        current_character_count = character_counts[index];

        if (requires_subtractive_notation(index, current_character_count)) {
            insert_subtractive_form(&current_position, index,
                                    current_character_count);
        } else {
            insert_copies_of_character(&current_position, index,
                                       current_character_count);
        }
    }

    return result;
}

static int requires_subtractive_notation(rc_index index, int count)
{
    return index != RCI_M && count >= 4;
}

static void insert_subtractive_form(char **location,
                                    rc_index index,
                                    int quantity)
{
    rc_index index_of_larger_character = index + 1;
    if (quantity > 4) {
        index_of_larger_character++;
    }

    (*location)[0] = roman_characters[index];
    (*location)[1] = roman_characters[index_of_larger_character];

    *location += 2;
}

static void insert_copies_of_character(char **location, rc_index character_index,
                                       int number_of_copies)
{
    memset(*location, roman_characters[character_index], number_of_copies);
    *location += number_of_copies;
}

static int sum_over_array(const int *array) {
    int sum = 0;
    size_t index;
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
