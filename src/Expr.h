#include "Token.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>

using std::vector;
using std::string;
using std::map;

#ifndef EXPR_H
#define EXPR_H

class Expr {
public:
    Expr(Type t = NONE) : left(NULL), right(NULL), value(NONE), type(t) {}
    Expr(const Expr &other);
    void setLeft(const Expr &other);
    void setRight(const Expr &other);
    void setValue(Token v) {
        value = v;
    }
    void setType(Type t) {
        type = t;
    }
    bool isNull() const {
        return type == NONE;
    }

    bool hasLeft() const {
        return left != NULL;
    }

    bool hasRight() const {
        return right != NULL;
    }

    const Expr &getRight() const {
        return *right;
    }

    const Expr &getLeft() const {
        return *left;
    }

    Type getType() const {
        return type;
    }

    bool isValue() const {
        return type == ID || type == NUM;
    }

    Token getValue() const {
        return value;
    }

    Expr &operator=(const Expr &rhs);
    friend ostream &operator<<(ostream &s, const Expr &expr);

    int eval(vector<int> record, map<string, int> indexes) const;
    ~Expr();
private:
    Expr *left;
    Expr *right;
    Token value;
    Type type;
};

const Expr NULL_EXPR;

#endif