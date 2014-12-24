#include <cctype>
#include <string>
#include <cstring>
#include <iostream>

using std::istream;
using std::string;

#define BUF_SIZE 256
class Lexer {

public:
    Lexer(istream &s) {
        line = 0;
        stream = s;
    }

    Token next () {
        while(stream.get(peek)) {
            if (isdigit(peek)) {  // numbers
                num_buffer = 0;
                do {
                    num_buffer = num_buffer * 10 + peek - '0';
                } while (stream.get(peek) && isdigit(peek));

                if (peek != EOF) {
                    stream.putback(peek);
                }

                return Token(NUM, &num_buffer, sizeof(int));
            } else if (isalpha(peek) || peek == '_') {  // keywords or identifilers
                memset(buffer, '\0');
                int cur = 0;
                do {
                    buffer[cur++] = peek;
                } while (cur < BUF_SIZE && stream.get(peek) && (isalnum(peek) || peek == '_'));

                if (cur == BUF_SIZE) {
                    throw LexError("Exceed maximun identifier length");
                }
                if (peek != EOF) {
                    stream.putback(peek);
                }

                string str(buffer);

                if (WORDS.find(str) != WORDS.end()) {  // keyword
                    return Token(WORDS[str]);
                } else {
                    return Token(ID, str.c_str(), str.size());
                }
            } else if (isspace(peek)) {  // white space
                if (peek == '\n')
                    line++;
            } else if (SINGLEOP.find(peek) != SINGLEOP.end()) {
                return Token(SINGLEOP[peek]);  // deterministic single character operators
            } else if (isOp(peek)) {  // operators
                memset(buffer, '\0');
                int cur = 0;
                do {
                    buffer[cur++] = peek;
                } while (cur < MAX_OP_SIZE && stream.get(peek) && isOp(peek));

                if (peek != EOF) {
                    stream.putback(peek);
                }

                string str(buffer);

                while (str.size() != 0 && OP.find(str) == OP.end()) { // too long
                    char temp = str[str.size() - 1]; // last charactor
                    str = str.substr(0, str.size() - 1);
                    stream.pushback(temp);
                }

                if (str.size() == 0) {
                    throw LexError("Invalid operator");
                }

                return Token(OP[str]);

            } else {  // error
                throw LexError("Invalid lexeme");
            }
        }
    }

private:
    char peek;
    char buffer[BUF_SIZE];
    int num_buffer;
    int line;  // line number
    istream stream;
};
