/* check_roman_calculator.c */

#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "../src/roman_calculator.h"

static void assert_sum_equals(const char *summand1, const char *summand2, const char *expected_sum);

START_TEST(addition_function_accepts_two_strings)
    char *sum = add_roman_numerals("I", "I");
    free(sum);
END_TEST

START_TEST(sum_of_I_and_I_is_II)
    assert_sum_equals("I", "I", "II");
END_TEST

START_TEST(sum_of_II_and_I_is_III)
    assert_sum_equals("II", "I", "III");
END_TEST

START_TEST(sum_of_V_and_V_is_X)
    assert_sum_equals("V", "V", "X");
END_TEST

START_TEST(sum_of_III_and_II_is_V)
    assert_sum_equals("III", "II", "V");
END_TEST

static void assert_sum_equals(const char *summand1, const char *summand2, const char *expected_sum) {
     char *sum = add_roman_numerals(summand1, summand2);
     ck_assert_str_eq(sum, expected_sum);
     free(sum);
}

Suite *create_calculator_test_suite(void)
{
    Suite *test_suite = suite_create("Roman_Calculator");

    TCase *addition_test_case = tcase_create("Addition");

    tcase_add_test(addition_test_case, addition_function_accepts_two_strings);
    tcase_add_test(addition_test_case, sum_of_I_and_I_is_II);
    tcase_add_test(addition_test_case, sum_of_II_and_I_is_III);
    tcase_add_test(addition_test_case, sum_of_V_and_V_is_X);
    tcase_add_test(addition_test_case, sum_of_III_and_II_is_V);
    tcase_add_test(addition_test_case, can_sum_and_carryover);

    suite_add_tcase(test_suite, addition_test_case);

    return test_suite;
}

int main(void)
{
    int failure_count = 0;
    Suite *test_suite = create_calculator_test_suite();
    SRunner *suite_runner = srunner_create(test_suite);
    srunner_run_all(suite_runner, CK_NORMAL);

    failure_count = srunner_ntests_failed(suite_runner);
    srunner_free(suite_runner);
    return (failure_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
