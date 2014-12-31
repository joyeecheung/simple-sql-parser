#include <cctype>
#include <string>
#include <cstring>
#include <iostream>
#include <map>

using std::istream;
using std::string;
using std::map;

#define BUF_SIZE 256
#define MAX_OP_SIZE 2

class LexError : std::exception {
public:
    LexError(string _msg) : msg(_msg) {}
    ~LexError() throw () {} // Updated
    const char *what() const throw() {
        return msg.c_str();
    }
private:
    string msg;
};

class Lexer {
public:
    Lexer(istream &s) : stream(s) {
        line = 0;
        initLookups();
    }

    void initLookups() {
        words["create"] = CREATE;
        words["table"] = TABLE;
        words["int"] = INT;
        words["default"] = DEFAULT;
        words["primary"] = PRIMARY;
        words["key"] = KEY;
        words["insert"] = INSERT;
        words["into"] = INTO;
        words["values"] = VALUES;
        words["delete"] = DELETE;
        words["from"] = FROM;
        words["where"] = WHERE;
        words["select"] = SELECT;

        singleOp['+'] = PLUS;
        singleOp['-'] = MINUS;
        singleOp['*'] = MUL;
        singleOp['/'] = DIV;
        singleOp['('] = L_PAREN;
        singleOp[')'] = R_PAREN;
        singleOp[','] = COMMA;
        singleOp[';'] = SEMICOLON;

        ops["&&"] = AND;
        ops["||"] = OR;
        ops["!"] = NOT;
        ops["<"] = LT;
        ops[">"] = GT;
        ops["!="] = NEQ;
        ops["=="] = EQ;
        ops[">="] = GEQ;
        ops["<="] = LEQ;
        ops["+"] = PLUS;
        ops["-"] = MINUS;
        ops["*"] = MUL;
        ops["/"] = DIV;
        ops["("] = L_PAREN;
        ops[")"] = R_PAREN;
        ops[","] = COMMA;
        ops[";"] = SEMICOLON;
    }

    bool isOp (char ch) {
        return ch == '&' || ch == '|' || ch == '!' || ch == '<'
               || ch == '>' || ch == '=' || ch == '+' || ch == '-'
               || ch == '*' || ch == '/' || ch == '(' || ch == ')'
               || ch == ',' || ch == ';';
    }

    char read_char() {
        stream.get(peek);
        col++;
        return peek;
    }

    // after returen, peek and buffers will retain recent values,
    // in particular, peek may contain values put back in the stream during last call
    // so next time next() is called, we need to intialize them properly
    Token next() {
        peek = stream.peek();
        while (peek != EOF) {
            read_char();
            if (isdigit(peek)) {  // numbers
                num_buffer = 0;
                do {
                    num_buffer = num_buffer * 10 + peek - '0';
                    read_char();
                } while (!isEnd() && isdigit(peek));

                if (peek != EOF) {
                    stream.putback(peek);
                }

                return Token(NUM, &num_buffer, sizeof(int));
            } else if (isalpha(peek) || peek == '_') {  // keywords or identifilers
                memset(buffer, '\0', sizeof(buffer));
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
                for (int i = 0; i < str.size(); ++i) {
                    str[i] = tolower(str[i]); // case insensitive
                }

                if (words.find(str) != words.end()) {  // keyword
                    return Token(words[str]);
                } else {
                    return Token(ID, str.c_str(), str.size());
                }
            } else if (isspace(peek)) {  // white space
                if (peek == '\n')
                    line++;
            } else if (singleOp.find(peek) != singleOp.end()) {
                return Token(singleOp[peek]);  // deterministic single character operators
            } else if (isOp(peek)) {  // operators
                memset(buffer, '\0', sizeof(buffer));
                int cur = 0;
                do {
                    buffer[cur++] = peek;
                } while (cur < MAX_OP_SIZE && stream.get(peek) && isOp(peek));

                if (peek != EOF) {
                    stream.putback(peek);
                }

                string str(buffer);

                while (str.size() != 0 && ops.find(str) == ops.end()) { // too long
                    char temp = str[str.size() - 1]; // last charactor
                    str = str.substr(0, str.size() - 1);
                    stream.putback(temp);
                }

                if (str.size() == 0) {
                    throw LexError("Invalid operator");
                }

                return Token(ops[str]);
            } else if (peek == EOF) {  // error
                return Token(END);  // $
            } else {  // error
                throw LexError("Invalid lexeme");
            }
        }
    }

    int getLine() {
        return line;
    }

    map<string, Type> words;
    map<char, Type> singleOp;
    map<string, Type> ops;
private:
    char peek;
    char buffer[BUF_SIZE];
    int num_buffer;
    int line;  // line number
    istream &stream;
};
