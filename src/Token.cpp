#include "Token.h"

map<Type, string> Token::name;

Token::Token(Type _type, const void *raw, const int size) {
    if (isValue(_type) && ((raw == NULL) || size == 0)) {
        throw TokenError("Expected non-keyword");
    }

    int real_size = size;
    if (_type == ID) {
        real_size++;  // '\0'
    }

    if (isValue(_type)) {
        data = (char *)malloc(real_size);
        memcpy(data, (char *)raw, real_size / sizeof(char));
    }
    type = _type;
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
}

// identifiers
string Token::getId() const {
    if (type != ID){
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

ostream & operator<<(ostream &s, const Token &token) {
    Type type = token.getType();
    string token_name = Token::name[type];

    if (type == ID) {
        s << "(" << token_name << ", " << token.getId() << ") ";
    } else if (type == NUM) {
        s << "(" << token_name << ", " << token.getNumber() << ") ";
    } else {
        s << token_name << " ";
    }
    return s;
}