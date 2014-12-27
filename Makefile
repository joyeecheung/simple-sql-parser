CC=clang++
CFLAGS=-c -Wall -DTRACK 

TOKEN=src/Token.cpp
LEXER=src/Lexer.cpp
MAIN=src/test_lexer.cpp

SOURCES=$(TOKEN) $(LEXER) $(MAIN)
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=bin/ssql

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -g -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -g -o $@

clean:
	rm src/*.o $(EXECUTABLE)

test: $(EXECUTABLE)
	bin/ssql test/lexer.in > test/lexer.out
	diff test/lexer.out test/lexer.good

.PHONY: test
