#include "Token.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>

#ifndef EXPR_H
#define EXPR_H

namespace ssql {

using std::vector;
using std::string;
using std::map;


class RuntimeError : std::exception {
public:
    RuntimeError(string _msg) : msg(_msg) {}
    ~RuntimeError() throw () {} // Updated
    const char *what() const throw() {
        return msg.c_str();
    }
private:
    string msg;
};

/* Expression class with a tree-like structure.
 *
 * There are three type of expressions:
 *     1. Binary expression. `left` and `right` are pointers to
 *        left and right expressions, `type` is the operator,
 *        `value` is a token with type `NONE`.
 *     2. Unary expression. `left` is NULL, `right` is the right operand,
 *        `type` is the unary operator, `value` is a token with type `NONE`.
 *     3. Operand. `type`is ID or NUM, `left` and `right` are NULL,
 *         `value` is the token for the operand.
 */
class Expr {
public:
    Expr(Type t = NONE) : left(NULL), right(NULL), value(NONE), type(t) {}
    Expr(const Expr &other);

    void setLeft(const Expr &other);
    void setRight(const Expr &other);
    // put other into the left of the
    // leftmost expression in the tree
    void setLeftMost(const Expr &other);

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

    // pass in the row record and id-to-index map
    // e.g. eval([1,2,3], {"sid": 0, "age": 1, "name": 2})
    int eval(vector<int> record, map<string, int> indexes) const;
    // for simple expressions i.e. no id binding
    int eval() const;

    ~Expr();
private:
    Expr *left;
    Expr *right;
    Token value;
    Type type;
};

const Expr NULL_EXPR;

}

#endif