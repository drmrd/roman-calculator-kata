all: build/roman_calculator.o build/libroman_calculator.a

dev: all check

build:
	mkdir build

build/roman_calculator.o: build
	gcc -g -Wall -c -Isrc src/roman_calculator.c -o build/roman_calculator.o

build/libroman_calculator.a: build/roman_calculator.o
	ar rcs build/libroman_calculator.a build/roman_calculator.o
	ranlib build/libroman_calculator.a

.PHONY: check
check: all
	gcc -g -Wall -Wextra tests/check_roman_calculator.c \
	-o tests/check_roman_calculator.o \
	build/libroman_calculator.a \
	`pkg-config --libs check`
	@echo ""
	@./tests/check_roman_calculator.o

clean:
	rm -rf build/
	rm -f tests/check_roman_calculator.o
