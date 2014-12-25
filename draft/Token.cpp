#include <string>
#include <cstring>
#include <iostream>
using std::string;
using std::cin;
using std::cout;

enum Type {
    NUM, ID, CREATE, INSERT, DELETE, SELECT
};

bool isKeyword(Type type) {
    return type != NUM && type != ID;
}

class LexError : std::exception {
public:
   LexError(string _msg) : msg(_msg) {}
   ~LexError() throw () {} // Updated
   const char* what() const throw() { return msg.c_str(); }
private:
    string msg;
};

class Token {
public:
    Token(Type _type, const void *raw=NULL, const int size=0) {
        if (!isKeyword(_type) && (raw == NULL) || size == 0) {
            throw LexError("Expected non-keyword");
        }

        int real_size = size;
        if (_type == ID) {
            real_size++;  // '\0'
        }

        if (_type == ID || _type == NUM) {
            data = (char *)malloc(real_size);
            memcpy(data, (char *)raw, real_size / sizeof(char));
        }
        type = _type;
    }

    // identifiers
    string getId() const {
        if (type != ID){
            throw LexError("Expected identifer, get otherwise");
        } else {
            return string(data);
        }
    }

    // keywords
    Type getKeyword() const {
        if (type == ID or type == NUM) {
            throw LexError("Expected keywords, get otherwise");
        } else {
            return type;
        }
    }

    // numbers
    int getNumber() const {
        if (type != NUM) {
            throw LexError("Expected number, get otherwise");
        } else {
            int result = 0;
            memcpy(&result, data, sizeof(int));
            return result;
        }
    }

    bool operator== (Type rhs) {
        return type == rhs;
    }

    Type getType() const {
        return type;
    }
private:
    Type type;
    char *data;
};

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

    } catch(LexError e) {
        cout << e.what() << '\n';
    }


    return 0;
}
