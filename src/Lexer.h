#include "Token.h"

#include <cstdio>
#include <cctype>

using std::istream;
using std::string;
using std::map;

#ifndef LEXER_H
#define LEXER_H

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
        line = 0;
        initLookups();
    }

    void initLookups() const;

    bool isOp (char ch) const;

    Token next();

    int getLine() const {
        return line;
    }

    bool isEnd() const {
        return stream.peek() == EOF;
    }

    int peekstream() const {
      return stream.peek();
    }

    static map<string, Type> words;
    static map<char, Type> singleOp;
    static map<string, Type> ops;
private:
    char peek;
    char buffer[BUF_SIZE];
    int num_buffer;
    int line;  // line number
    istream &stream;
};

#endif