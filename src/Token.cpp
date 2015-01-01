#include "Token.h"

namespace ssql {

map<Type, string> Token::name;

Token::Token(Type _type, const void *raw, const int size) {
    if (isValue(_type)) {  // number or identifier
        if (raw == NULL || size == 0) {
            throw TokenError("Expected non-keyword");
        } else {
            type = _type;

            real_size = size / sizeof(char);
            if (type == ID) {
                real_size++;  // '\0'
            }

            data = new char[real_size];
            memcpy(data, (char *)raw, real_size);
        }
    } else {  // keyword or operator
        type = _type;
        data = NULL;
        real_size = 0;
    }
}

Token::Token(const Token &other) {
    type = other.type;
    if (isValue(type)) {  // number or identifier
        real_size = other.real_size;
        data = new char[real_size];
        memcpy(data, other.data, real_size);
    } else {  // keyword or operator
        real_size = 0;
        data = NULL;
    }
}

void Token::initNameMap() {
    name[ID] = "ID";
    name[NUM] = "NUM";
    name[CREATE] = "CREATE";
    name[TABLE] = "TABLE";
    name[INT] = "INT";
    name[DEFAULT] = "DEFAULT";
    name[PRIMARY] = "PRIMARY";
    name[KEY] = "KEY";
    name[INSERT] = "INSERT";
    name[INTO] = "INTO";
    name[VALUES] = "VALUES";
    name[DELETE] = "DELETE";
    name[FROM] = "FROM";
    name[WHERE] = "WHERE";
    name[SELECT] = "SELECT";
    name[ASSIGN] = "ASSIGN";
    name[LT] = "LT";
    name[GT] = "GT";
    name[NEQ] = "NEQ";
    name[EQ] = "EQ";
    name[GEQ] = "GEQ";
    name[LEQ] = "LEQ";
    name[PLUS] = "PLUS";
    name[MINUS] = "MINUS";
    name[MUL] = "MUL";
    name[DIV] = "DIV";
    name[AND] = "AND";
    name[OR] = "OR";
    name[NOT] = "NOT";
    name[L_PAREN] = "L_PAREN";
    name[R_PAREN] = "R_PAREN";
    name[COMMA] = "COMMA";
    name[SEMICOLON] = "SEMICOLON";
    name[END] = "END";
    name[NONE] = "NONE";
}

// identifiers
string Token::getId() const {
    if (type != ID) {
        throw TokenError("Expected identifer, get otherwise");
    } else {
        return string(data);
    }
}

// keywords
Type Token::getKeyword() const {
    if (isValue(type)) {
        throw TokenError("Expected keywords, get otherwise");
    } else {
        return type;
    }
}

// numbers
int Token::getNumber() const {
    if (type != NUM) {
        throw TokenError("Expected number, get otherwise");
    } else {
        int result = 0;
        memcpy(&result, data, sizeof(int));
        return result;
    }
}

ostream &operator<<(ostream &s, const Token &token) {
    if (Token::name.size() == 0) {
        Token::initNameMap();
    }

    Type type = token.type;
    string token_name = Token::name[type];

    if (type == ID) {
        s << "(" << token_name << ", " << token.getId() << ")";
    } else if (type == NUM) {
        s << "(" << token_name << ", " << token.getNumber() << ")";
    } else {
        s << token_name;
    }
    return s;
}

Token &Token::operator=(const Token &rhs) {
    type = rhs.type;

    // release old data
    if (data != NULL) {
        delete [] data;
        data = NULL;
        real_size = 0;
    }

    if (isValue(type)) {  // number or identifier
        real_size = rhs.real_size;
        data = new char[real_size];
        memcpy(data, rhs.data, real_size);
    } else {  // keyword or operator
        real_size = 0;
        data = NULL;
    }
    return *this;
}

Token::~Token() {
    if (data != NULL) {
        delete [] data;
        data = NULL;
    }
}

}