#include "Token.h"

#include <cstdio>
#include <cctype>

#ifndef LEXER_H
#define LEXER_H

namespace ssql {

using std::istream;
using std::string;
using std::map;

#define BUF_SIZE 256
#define MAX_OP_SIZE 2

class LexError : std::exception {
public:
   LexError(string _msg) : msg(_msg) {}
   ~LexError() throw () {} // Updated
   const char* what() const throw() { return msg.c_str(); }
private:
    string msg;
};

class Lexer {
public:
    Lexer(istream &s) : stream(s) {
        line = 1;
        col = 1;
        initLookups();
    }

    bool isOp (char ch) const;

    Token next();

    int getCol() const {
        return col;
    }

    int getLine() const {
        return line;
    }

    bool isEnd() {
        peek = stream.peek();
        return peek == EOF;
    }

    char advance() {
        char last = stream.get();
        peek = stream.peek();
        col++;
        return last;
    }

    void retreat(char ch) {
        stream.putback(ch);
        col--;
        peek = ch;
    }

    static map<string, Type> words;
    static map<char, Type> singleOp;
    static map<string, Type> ops;
private:
    static void initLookups();
    char peek;
    char buffer[BUF_SIZE];
    int num_buffer;
    int line;  // line number
    int col; // column number
    istream &stream;
};

}

#endif