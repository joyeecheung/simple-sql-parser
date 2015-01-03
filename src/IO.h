#include <iostream>
#include <iomanip>

#include "Token.h"
#include "Lexer.h"
#include "Parser.h"
#include "Engine.h"
#include "Expr.h"

#ifndef IO_H
#define IO_H

namespace ssql {

#define WIDTH_PER_COL 15
using std::cout;
using std::setw;

class IO {
public:
    IO(Parser &p, Lexer &l, Engine &e) : parser(p), lexer(l), engine(e) {}
    // print a line of ch with given length
    void printLine(int length, char ch = '-');
    // print the data inside  given results and column names
    void printData(const vector<string> &names,
                    vector<vector<int> > &results);
    // loop to execute statements
    void loop();
    // execute statements, output error prompts
    void execute(Type next);
private:
    Parser &parser;
    Lexer &lexer;
    Engine &engine;
};

}

#endif