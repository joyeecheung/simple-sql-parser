#include <iostream>
#include <fstream>

#include "Token.h"
#include "Lexer.h"

using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;

using namespace ssql;

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
    Token lookahead = lexer.next();
    while (lookahead != END) {
        try {
            cout << lookahead << '\n';
            lookahead = lexer.next();
        } catch (LexError e) {
            cout << e.what() << '\n';
        }
    }

    if (lookahead == END) {
        cout << lookahead << '\n';
    }
    delete lexptr;
    return 0;
 }
