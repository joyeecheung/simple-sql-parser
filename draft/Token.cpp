#include <string>
#include <cstring>
#include <iostream>
#include <map>

using std::string;
using std::cin;
using std::cout;

enum Type {
    ID, NUM, CREATE, TABLE, INT, DEFAULT, PRIMARY, KEY, INSERT,
    INTO, VALUES, DELETE, FROM, WHERE, SELECT, LT, GT, NEQ, EQ,
    GEQ, LEQ, PLUS, MINUS, MUL, DIV, AND, OR, NOT, L_PAREN, R_PAREN,
    COMMA, SEMICOLON, END
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
    Token(Type _type, const void *raw=NULL, const int size=0) {
        if (isValue(_type) && (raw == NULL) || size == 0) {
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

    bool isValue(Type t) const {  // number or identifier
        return t == NUM || t == ID;
    }

    void initNameMap() {
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
    string getId() const {
        if (type != ID){
            throw TokenError("Expected identifer, get otherwise");
        } else {
            return string(data);
        }
    }

    // keywords
    Type getKeyword() const {
        if (isValue(type)) {
            throw TokenError("Expected keywords, get otherwise");
        } else {
            return type;
        }
    }

    // numbers
    int getNumber() const {
        if (type != NUM) {
            throw TokenError("Expected number, get otherwise");
        } else {
            int result = 0;
            memcpy(&result, data, sizeof(int));
            return result;
        }
    }

    bool operator== (Type rhs) {
        return type == rhs;
    }

    friend ostream &operator<<(ostream &s, const Token &token) {
        s << name[type] << ": ";
        if (type == ID) {
            s << "(" << name[type] << ", " << getId() << " ) ";
        } else if (type == NUM) {
            s << "(" << name[type] << ", " << getNumber() << " ) ";
        } else {
            s << name[type] << " ";
        }
    }

    Type getType() const {
        return type;
    }

    static map<Type, string> name;
private:
    Type type;
    char *data;
};

/*
int main(void) {
    // usage:
    int v = 15;
    string s("hey!");
    Type id = SELECT;
    const char *a = s.c_str();

    try {
        Token token(NUM, &v, sizeof(int));
        int value = token.getNumber();
        cout << value << '\n';
        cout << s.size() << '\n';

        Token token2(ID, s.c_str(), s.size());
        string str = token2.getId();
        cout << str << '\n';

        Token token3(SELECT, s.c_str(), s.size());
        cout << (token3.getKeyword() == SELECT) << '\n';

        Token token4(SELECT, s.c_str(), s.size());
        string str2 = token3.getId();  // throw expection
        cout << str2 << '\n';

    } catch(TokenError e) {
        cout << e.what() << '\n';
    }


    return 0;
}
*/
