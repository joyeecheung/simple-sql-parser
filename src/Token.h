#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <map>

using std::string;
using std::ostream;
using std::map;

#ifndef TOKEN_H
#define TOKEN_H

enum Type {
    ID, NUM, CREATE, TABLE, INT, DEFAULT, PRIMARY, KEY, INSERT,
    INTO, VALUES, DELETE, FROM, WHERE, SELECT, ASSIGN, LT, GT, NEQ, EQ,
    GEQ, LEQ, PLUS, MINUS, MUL, DIV, AND, OR, NOT, L_PAREN, R_PAREN,
    COMMA, SEMICOLON, END, NONE
};

class TokenError : std::exception {
public:
   TokenError(string _msg) : msg(_msg) {}
   ~TokenError() throw () {} // Updated
   const char* what() const throw() { return msg.c_str(); }
private:
    string msg;
};

// avoid RTTI since Token's value will be frequently needed
class Token {
public:
    Token(Type _type=NONE, const void *raw=NULL, const int size=0);

    // number or identifier
    bool isValue(Type t) const {  // number or identifier
        return t == NUM || t == ID;
    }

    Type getType() const {
        return type;
    }

    // identifiers
    string getId() const;
    // keywords
    Type getKeyword() const;
    // numbers
    int getNumber() const;

    friend ostream &operator<<(ostream &s, const Token &token);
    bool operator== (Type rhs) const {
        return type == rhs;
    }
    bool operator!= (Type rhs) const {
        return !(*this == rhs);
    }
    Token &operator=(const Token &rhs);

    ~Token();
    static map<Type, string> name;
private:
    static void initNameMap();
    Type type;
    char *data;
    int real_size;
};

#endif