#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <map>

#ifndef TOKEN_H
#define TOKEN_H

namespace ssql {

using std::string;
using std::ostream;
using std::map;

// keywords and operators
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
    Token(const Token &other);

    // number or identifier
    bool isValue(Type t) const {  // number or identifier
        return t == NUM || t == ID;
    }

    Type getType() const {
        return type;
    }

    // For identifiers. Need to guarded by getType() == ID
    string getId() const;
    // For keywords or operators. Need to guarded by !isValue(getType())
    Type getKeyword() const;
    // For numbers. Need to guarded by getType() == NUM
    int getNumber() const;

    // Check type
    bool operator== (Type rhs) const {
        return type == rhs;
    }
    bool operator!= (Type rhs) const {
        return !(*this == rhs);
    }

    Token &operator=(const Token &rhs);
    friend ostream &operator<<(ostream &s, const Token &token);

    ~Token();

    static map<Type, string> name;  // uppercase name of types

private:
    static void initNameMap();  // statically init names

    Type type;  // type of the token
    char *data;  // raw data
    int real_size;  // size of data[]
};

}

#endif