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
