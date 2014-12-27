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
    if (argc > 1) {
        ifstream in(argv[1], ifstream::in);
        lexptr = new Lexer(in);
    } else {
        lexptr = new Lexer(cin);
    }
    Lexer &lexer = *lexptr;
    Token::initNameMap();

    while (!lexer.isEnd()) {
        cout << lexer.next() << '\n';
    }
    return 0;
 }
