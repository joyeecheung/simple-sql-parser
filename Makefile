CC=clang++
#CFLAGS=-c -Wall -DTRACK -std=c++11
CFLAGS=-c -Wall -std=c++11

TOKEN=src/Token.cpp
LEXER=src/Lexer.cpp
EXPR=src/Expr.cpp
PARSER=src/Parser.cpp
TABLE=src/Table.cpp
ENGINE=src/Engine.cpp

LEXMAIN=src/test_lexer.cpp
PARSEMAIN=src/test_parser.cpp
MAIN=src/main.cpp

LEXIN=test/lexer.in
PARSEIN=test/parser.in
ALLIN=test/all.in

SOURCES=$(TOKEN) $(LEXER) $(EXPR) $(PARSER) $(TABLE) $(ENGINE)
OBJECTS=$(SOURCES:.cpp=.o)

MAINOBJ=$(MAIN:.cpp=.o)
LEXMAINOBJ=$(LEXMAIN:.cpp=.o)
PARSEMAINOBJ=$(PARSEMAIN:.cpp=.o)

EXECUTABLE=bin/ssql

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(MAINOBJ)
	$(CC) $(OBJECTS) $(MAINOBJ) -g -o $@

$(OBJECTS) : $(SOURCES)

$(MAINOBJ) : $(MAIN)

$(LEXMAINOBJ) : $(LEXMAIN)

$(PARSEMAINOBJ) : $(PARSEMAIN)

%.o: %.cpp
	$(CC) $(CFLAGS) $< -g -o $@

clean:
	rm -f src/*.o $(EXECUTABLE)

testparser: $(OBJECTS) $(PARSEMAINOBJ) $(PARSEIN)
	$(CC) $(OBJECTS) $(PARSEMAINOBJ) -g -o $(EXECUTABLE)
	$(EXECUTABLE) $(PARSEIN)

testlexer: $(OBJECTS) $(LEXMAINOBJ) $(LEXIN)
	$(CC) $(OBJECTS) $(LEXMAINOBJ) -g -o $(EXECUTABLE)
	$(EXECUTABLE) $(LEXIN) > test/lexer.out
	diff test/lexer.out test/lexer.good

checkmem: $(testparser)
	valgrind --leak-check=full -v $(EXECUTABLE) $(PARSEIN)

test: $(EXECUTABLE)
	$(EXECUTABLE) $(ALLIN)

.PHONY: testparser testlexer checkmem test
