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
    void printLine(int length, char ch = '-');
    void printData(const vector<string> &names,
                    vector<vector<int> > &results);
    void loop();
    void execute(Type next);
private:
    Parser &parser;
    Lexer &lexer;
    Engine &engine;
};

}

#endif