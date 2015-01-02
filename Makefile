CC=clang++
CXXFLAGS=-c -Wall -std=c++11

#For debug
#CXXFLAGS=-c -Wall -DTRACK -std=c++11

#For environment without clang++
#CC=g++
#CFLAG=-c -Wall -std=c++0x

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
	$(CC) $(CXXFLAGS) $< -g -o $@

clean:
	rm -f src/*.o $(EXECUTABLE)

testparser: $(OBJECTS) $(PARSEMAINOBJ) $(PARSEIN)
	$(CC) $(OBJECTS) $(PARSEMAINOBJ) -g -o $(EXECUTABLE)
	$(EXECUTABLE) $(PARSEIN) > test/parser.out
	diff test/parser.out test/parser.good

testlexer: $(OBJECTS) $(LEXMAINOBJ) $(LEXIN)
	$(CC) $(OBJECTS) $(LEXMAINOBJ) -g -o $(EXECUTABLE)
	$(EXECUTABLE) $(LEXIN) > test/lexer.out
	diff test/lexer.out test/lexer.good

checkmem: $(EXECUTABLE)
	valgrind --leak-check=full -v $(EXECUTABLE) $(ALLIN)

test: $(EXECUTABLE)
	$(EXECUTABLE) $(ALLIN)

.PHONY: testparser testlexer checkmem test
