#include "Expr.h"

Expr::Expr(const Expr &other) {
    value = other.value;
    type = other.type;
    left = right = NULL;

    if (other.left != NULL) {  // other has a left
        setLeft(*(other.left));
    } else {  // others doesn't have left
        left = NULL;
    }

    if (other.right != NULL) {
        setRight((*other.right));
    } else {
        right = NULL;
    }
}

void Expr::setLeft(const Expr &other) {
    if (left != NULL) {
        delete left;
        left = NULL;
    }

    left = new Expr(other.type);
    *left = other;
}

void Expr::setRight(const Expr &other) {
    if (right != NULL) {
        delete right;
        right = NULL;
    }

    right = new Expr(other.type);
    *right = other;
}

Expr &Expr::operator=(const Expr &rhs) {
    if (rhs.left != NULL) {  // other has a left
        setLeft(*(rhs.left));
    } else {  // others doesn't have left
        if (left != NULL) {
            delete left;
            left = NULL;
        }
    }

    if (rhs.right != NULL) {
        setRight((*rhs.right));
    } else {
        if (right != NULL) {
            delete right;
            right = NULL;
        }
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
        left = NULL;
    }

    if (right != NULL) {
        delete right;
        right = NULL;
    }
}
