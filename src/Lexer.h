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

    // can't be read anymore i.e. peek == EOF
    bool isEnd() {
        peek = stream.peek();
        return peek == EOF;
    }

    // advance in the stream, return the read character
    char advance() {
        char last = stream.get();
        peek = stream.peek();
        col++;  // record column
        return last;
    }

    void retreat(char ch) {  // put ch back to stream
        stream.putback(ch);
        col--;  // record column
        peek = ch;
    }

    static map<string, Type> words;  // keywords
    // operators that can be determined by one character
    static map<char, Type> singleOp;
    static map<string, Type> ops;  // other operators
private:
    static void initLookups();  // init the maps

    char peek;  // next character to read by advance()
    char buffer[BUF_SIZE];  // for keywords, operators etc.
    int num_buffer;  // for accumulating numbers
    int line;  // line number
    int col; // column number
    istream &stream;  // stream for lexer
};

}

#endif