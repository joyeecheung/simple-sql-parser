#include "Expr.h"

void Expr::setLeft(const Expr &other) {
    if (left != NULL) {
        delete left;
    }

    left = new Expr(other.type);
    *left = other;
}

void Expr::setRight(const Expr &other) {
    if (right != NULL) {
        delete right;
    }

    right = new Expr(other.type);
    *right = other;
}

Expr & Expr::operator=(const Expr &rhs) {
    if (rhs.left != NULL) {
        setLeft(*(rhs.left));
    } else {
        left = NULL;
    }

    if (rhs.right != NULL) {
        setRight((*rhs.right));
    } else {
        right = NULL;
    }

    value = rhs.value;
    type = rhs.type;
    return *this;
}


ostream &operator<<(ostream &s, const Expr &expr) {
    if (expr.isValue()) {  // id or number
        s << expr.getValue();
        return s;
    }

    // real expression
    s << "(";
    if (expr.hasLeft()) {
        s << expr.getLeft();
    }

    if (!expr.isNull()) {  // operator
        s << " " << Token(expr.getType()) << " ";
    }

    if (expr.hasRight()) {
        s << expr.getRight();
    }

    s << ")";
    return s;
}

int Expr::eval(vector<int> record, map<string, int> indexes) const {
    if (type == NUM) {
        return value.getNumber();
    } else if (type == ID) {
        return record[indexes[value.getId()]];
    } else if (type == AND) {
        return left->eval(record, indexes) && right->eval(record, indexes);
    } else if (type == EQ) {
        return left->eval(record, indexes) == right->eval(record, indexes);
    } else if (type == NEQ) {
        return left->eval(record, indexes) != right->eval(record, indexes);
    } else if (type == LT) {
        return left->eval(record, indexes) < right->eval(record, indexes);
    } else if (type == GT) {
        return left->eval(record, indexes) > right->eval(record, indexes);
    } else if (type == LEQ) {
        return left->eval(record, indexes) <= right->eval(record, indexes);
    } else if (type == GEQ) {
        return left->eval(record, indexes) >= right->eval(record, indexes);
    } else if (type == NONE) {
        return true;
    }
    return 0;
}

Expr::~Expr() {
    if (left != NULL) {
        delete left;
    }

    if (right != NULL) {
        delete right;
    }
}
