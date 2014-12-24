#include <string>
#include <cstring>
#include <iostream>
using std::string;
using std::cin;
using std::cout;

enum Type {
    NUM, ID, CREATE, INSERT, DELETE, SELECT
};

class ParseError : std::exception {
public:
   ParseError(string _msg) : msg(_msg) {}
   ~ParseError() throw () {} // Updated
   const char* what() const throw() { return msg.c_str(); }
private:
    string msg;
};

class Token {
public:
    Token(Type _type, const void *raw, int size) {
        if (_type == ID) {
            data = (char *)malloc(size + 1);  // trailing '\0'
        } else {
            data = (char *)malloc(size);
        }
        memcpy(data, (char *)raw, size / sizeof(char));
        type = _type;
    }

    // identifiers
    string getId() const {
        if (type != ID){
            throw ParseError("Expected identifer, get otherwise");
        } else {
            return string(data);
        }
    }

    // keywords
    Type getKeyword() const {
        if (type == ID or type == NUM) {
            throw ParseError("Expected keywords, get otherwise");
        } else {
            return type;
        }
    }

    // numbers
    int getNumber() const {
        if (type != NUM) {
            throw ParseError("Expected number, get otherwise");
        } else {
            int result = 0;
            memcpy(&result, data, sizeof(int));
            return result;
        }
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

        Token token2(ID, s.c_str(), s.size()*sizeof(char) + 1);
        string str = token2.getId();
        cout << str << '\n';

        Token token3(SELECT, s.c_str(), s.size()*sizeof(char) + 1);
        cout << (token3.getKeyword() == SELECT) << '\n';

        Token token4(SELECT, s.c_str(), s.size()*sizeof(char) + 1);
        string str2 = token3.getId();  // throw expection
        cout << str2 << '\n';

    } catch(ParseError e) {
        cout << e.what() << '\n';
    }


    return 0;
}
