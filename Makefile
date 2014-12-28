CC=clang++
CFLAGS=-c -Wall -DTRACK -std=c++11

TOKEN=src/Token.cpp
LEXER=src/Lexer.cpp
EXPR=src/Expr.cpp
PARSER=src/Parser.cpp
#MAIN=src/test_lexer.cpp
MAIN=src/test_parser.cpp

SOURCES=$(TOKEN) $(LEXER) $(EXPR) $(PARSER) $(MAIN)
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=bin/ssql

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -g -o $@

$(OBJECTS) : $(SOURCES)

%.o: %.cpp
	$(CC) $(CFLAGS) $< -g -o $@

clean:
	rm -f src/*.o $(EXECUTABLE)

test: $(EXECUTABLE)
	#bin/ssql test/lexer.in > test/lexer.out
	#diff test/lexer.out test/lexer.good
	bin/ssql test/parser.in

.PHONY: test
