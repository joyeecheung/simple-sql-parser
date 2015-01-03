## Dependencies
1. `clang++` 3.4 or higher / `g++` 4.6.3 or higher (needs `C++11/C++0x` support)
2. GNU Make
3. Linux

## How to build the project

If you have `clang++` 3.4 or higher installed, and it can be invoke with `clang++`, you don't need to change the `Makefile`.

If your `clang++` can't be invoked with `clang++`, e.g. has to be invoked with `clang++-3.4`, you need to change the first line of `Makefile`

	CC=clang++

into 

	CC=clang++-3.4

If you only have `g++`, uncomment these lines in the `Makefile` 

    CC=g++
    CXXFLAGS=-c -Wall -std=c++0x

If you still can't build the program, contact the authors. (See "About" at the end of this README).

After configuring the `Makefile`, enter the project root directory, run `make`

	$ make

The built program wil show up under `bin` directory as `bin/ssql`

	$ ls bin
	ssql

## How to pass inputs

To pass inputs into the program, under the root directory, run

	$ bin/ssql <path-to-input>

For example, to pass `test/all.in`, run

	$ bin/ssql test/all.in

Or under the `bin` directory, run

	$ ./ssql ../test/all.in

For interactive mode, simply run the program without arguments

	$ bin/ssql

and the standard input stream will be used as input.

### How to run the tests

To test the lexer

	$ make testlexer

To test the parser

	$ make testparser

To test the whole project

	$ make test

If you have valgrind installed, you can check the memory usage with

	$ make checkmem


##Directory structure
    |-- Makefile
    |-- README.md
    |
    |-- bin
    |   `-- ssql (the program. will show up after make)
    |
    |-- doc (documents)
    |   |-- design.pdf
    |   `-- test.pdf
    |
    |-- src  (sorce code)
    |   |-- Engine.cpp
    |   |-- Engine.h
    |   |-- Expr.cpp
    |   |-- Expr.h
    |   |-- IO.cpp
    |   |-- IO.h
    |   |-- Lexer.cpp
    |   |-- Lexer.h
    |   |-- Parser.cpp
    |   |-- Parser.h
    |   |-- Statements.cpp
    |   |-- Statements.h
    |   |-- Table.cpp
    |   |-- Table.h
    |   |-- Token.cpp
    |   |-- Token.h
    |   |-- main.cpp
    |   |-- test_lexer.cpp
    |   `-- test_parser.cpp
    |
    `-- test  (test cases)
        |-- all.good
        |-- all.in
        |-- lexer.good
        |-- lexer.in
        |-- parser.good
        `-- parser.in


##About
* [Github repository](https://github.com/joyeecheung/simple-sql-parser)
* Time: Jan. 2015
* Contact: [joyeec9h3@gmail.com](mailto:joyeec9h3@gmail.com)