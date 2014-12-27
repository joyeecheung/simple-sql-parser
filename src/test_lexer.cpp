#include <iostream>
#include <fstream>

#include "Token.h"
#include "Lexer.h"

using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;

int main(int argc, char const *argv[]) {
    Lexer *lexptr;
    ofstream out;
    ifstream in;  // must be in the scope until stop scanning

    if (argc > 1) {
        in.open(argv[1], ifstream::in);
        if (in.is_open()) {
            lexptr = new Lexer(in);
        } else {
            cout << "Fail to open " << argv[1] << '\n';
            exit(1);
        }
    } else {
        lexptr = new Lexer(cin);
    }

    Lexer &lexer = *lexptr;
    Token::initNameMap();

    while (!lexer.isEnd()) {
        try {
            cout << lexer.next() << '\n';
        } catch (LexError e) {
            cout << e.what() << '\n';
        }
    }
    return 0;
 }
