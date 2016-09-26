# Roman Numeral Calculator
This repository contains a solution to the Roman Calculator Kata written in
C. Upon compilation it provides a `roman_calculator` library with two functions

    add_roman_numerals(A, B)

and

    subtract_roman_numerals(A, B)

That take as input two Roman numerals `A` and `B` written as strings and return
the sum `A` + `B` and difference `A` - `B` (respectively), as dynamically
allocated strings.

## Terminology
Throughout the code I use standard terminology about Roman numerals, such as
"subtractive" and "additive" representations of these numbers. All of the
concepts are discussed on
[Wikipedia](https://en.wikipedia.org/wiki/Roman_numerals#Roman_numeric_system).

# Instructions/Make Targets
The project's `Makefile` should be called from its root directory, and provides
the following targets:

  * `all` (default):
    Compiles `src/roman_calculator.c` into the archive file
    `build/libroman_calculator.a`
  * `check`:
    Compiles and runs through the [Check](https://libcheck.github.io/check/)
    unit tests found in `tests/check_roman_calculator.c`.
  * `dev`:
    Runs the `all` recipe followed by `check`.
  * `install`:
    Compiles and installs the calculator library in a library directory of your
    choosing (specified by setting the `PREFIX` environment variable) or
    `/usr/local/lib` by default.

## Tested Environments
  * Written on a PC running Linux Mint 17.3 Rosa.
  * Tested on a fresh virtual machine install of Ubuntu 14.04 with
    `git`, `build-essentials`, `pkg-config`, and `check` packages installed.

# Implementation Notes

  * The instructions for this C kata say that

    > As we are in Rome there is no such thing as decimals or int, we need to do
    > this with [...] strings.

    In my solution, I've roughly interpreted this instruction as

    > Don't calculate A +/- B by converting A and B to decimal, performing +/-
    > on the resulting `int`s, and converting back. Finish the kata without
    > relying on special properties of place-value number systems or existing
    > addition/subtraction functions for built-in types.

  * The definition of Roman numerals in the instructions is not restrictive
    enough to make it so that there is a unique representation of each number as
    a Roman numeral. For example, both "CMXCIX" and "IM" can be used to
    represent 999 based on the rules described in the kata. My addition and
    subtraction functions always output a Roman numeral meeting all of the rules
    from the instructions and, at points where the instructions are ambiguous,
    appeal to modern conventions. For instance, if `A` and `B` are two of the
    characters `'I'`, `'V'`, `'X'`, `'L'`, `'C'`, `'D'`, and `'M'` such that `A`
    is less than `B`, the subtractive form `"AB"` can only appear as a substring
    of the output of one of these functions if the value of `A` is a power of 10
    (`'I'`, `'X'`, or `'C'`) and `B` is worth at most ten times the value of `A`
    (so `'V'` or `'X'` if `A == 'I'`, `'L'` or `'C'` if `A == 'X'`, etc.).

  * As I wrote this code as my solution to a kata, I focused primarily on the
    main algorithm itself and not, for example, validating user input. Currently
    both arithmetic functions call a `validate_user_input` function that simply
    exits early with an error if either input does not consist entirely of
    the "Roman characters" `'I'`, `'V'`, `'X'`, `'L'`, `'C'`, `'D'`, and `'M'`.
    Ensuring that each input string is `'\0'`-terminated and obeys the rules of
    being a Roman numeral would both be important (and straightforward)
    additions here before exposing these functions to users.

    On the other hand, this means that the calculator is quite flexible in terms
    of its input, and any unambiguous representation of a Roman numeral can be
    used as input with predictable results. So, for instance, the calculator is
    perfectly happy to accept `"IIIIIIIII"` instead of `"IX"`, but it is not
    designed to attempt to handle an ambiguous numeral such as `"IVX"`, which
    could be used to denote either 6 = 10 - (5 - 1) or 4 = (10 - 5) - 1. Again,
    however, error handling in the ambiguous cases could easily be added.

# Notes on Unit Testing

Due to time constraints I decided to limit my unit testing in this kata to tests
that allowed me to reach a finished product. That said, if I was intending to
maintain this project long term, a more robust set of unit tests including all
of the following would be. Below I've listed a couple thoughts on how one could
expand the test framework before doing major refactoring/releasing this code in
a mission critical setting.

  * Input validation tests for this exercise were my lowest priority, since they
    aren't really called for explicitly in the prompt, but I at least created a
    stub function `validate_input_strings` that currently just checks that input
    strings do not contain (non-terminal) characters other than the seven
    symbols `'I'`, `'V'`, ..., and `'M'` that are permitted in a valid Roman
    numeral. This should be expanded with additional tests that verify the input
    strings are valid representations of Roman numerals (and not, for instance,
    the ambiguous Roman numerals I mentioned earlier). Of course, to make this
    library secure we should probably switch to something other than C strings
    entirely (and, at very least, make a reasonable attempt at checking that
    input strings are null terminated), but that's a subject unto itself.

  * With our Roman numeral rules, every numeral exceeding 1000 is the
    concatenation of a string of `'M'` characters followed by a numeral with
    value less than 1000. Ignoring validating input, one can accordingly be
    pretty confident in an implementation of the `add_` and
    `subtract_roman_numerals` functions so long as

      1. the values of `add_` and `subtract_roman_numerals(X, Y)` are correct
         for Roman numerals `X`, `Y` with values up to 1000, and
      2. these functions correctly carryover when the input is greater than 1000,
         supporting prepending arbitrarily many M's to the output.
         
    Assuming 1. has been verified, test coverage for point 2. can be reasonably
    comprehensive with only a handful of tests.  Regarding point 1., a brute
    force test of the addition function for all valid input pairs representing
    values at most 1,000 can be achieved in 1,000,000 tests or even 1000 * 1001
    / 2 = 500500 tests assuming that `add_roman_numerals(X, Y) ==
    add_roman_numerals(Y, X)` can be verified to our satisfaction through other
    means.  Such a small amount of computations can be easily tested (perhaps
    through a less-frequently-run alternative test file/suite) with any modern
    computer.  Out of curiosity, I brute force tested both functions on my 2008
    ThinkPad, and (in addition to confirming the algorithms works as intended!)
    the tests each ran in under 5 seconds.  That number remained below 30
    seconds for much larger upper bounds on the input values.
    
    Because of this one could either replace the vast majority of the tests in
    our current suites with a brute force method or (better yet) provide a more
    comprehensive second test file containing a brute force check that could be
    less frequently.  My preference would be to speed up this test by minimizing
    redundancy, since it's clear here that the total number of assertions could
    be reduced from the brute force test by several orders of magnitude without
    noticeably impacting test coverage. Regardless, I thought it worth pointing
    out that, thanks to how Roman numerals represent integers, this is one of
    the unusual moments where a brute force testing suite is tractable.
