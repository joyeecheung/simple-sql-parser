#include <iostream>
#include <fstream>

#include "IO.h"

using std::cin;
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

    Parser parser(*lexptr);
    Engine engine;
    IO io(parser, *lexptr, engine);

    // try-catch in case the first lexeme is invalid
    try {
        io.loop();
    } catch (LexError e) {
        cout << "line " << lexptr->getLine() << ", ";
        cout << "column " << lexptr->getCol() << ": ";
        cout << e.what()  << '\n';
    } catch (ParseError e) {
        cout << "line " << parser.getLine() << ", ";
        cout << "column " << parser.getCol() << ": ";
        cout << e.what() << '\n';
    }

    delete lexptr;
    return 0;
}