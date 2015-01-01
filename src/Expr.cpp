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

void Expr::setLeftMost(const Expr &other) {
    Expr *cur = this;
    while(cur->hasLeft()) {
        cur = cur->left;
    }
    cur->setLeft(other);
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
    } else if (type == PLUS) {
        if (hasLeft()) {
            return left->eval(record, indexes) + right->eval(record, indexes);
        } else {
            right->eval(record, indexes);
        }
    } else if (type == MINUS) {
        if (hasLeft()) {
            return left->eval(record, indexes) - right->eval(record, indexes);
        } else {
            return 0 - right->eval(record, indexes);
        }
    } else if (type == MUL) {
        return left->eval(record, indexes) * right->eval(record, indexes);
    } else if (type == DIV) {
        // TODO: div by zero
        return left->eval(record, indexes) / right->eval(record, indexes);
    } else if (type == OR) {
        return left->eval(record, indexes) || right->eval(record, indexes);
    } else if (type == NOT) {
        return ! right->eval(record, indexes);
    } else if (type == NONE) {
        return true;  // for empty where
    }
    return 0;
}

int Expr::eval() const {
    vector<int> record;
    map<string, int> indexes;
    return eval(record, indexes);  // empty
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
