#include <iostream>
#include <fstream>

#include "Token.h"
#include "Lexer.h"
#include "Parser.h"
#include "Expr.h"

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

    Parser parser(*lexptr);
    while (!parser.isEnd()) {
        try {
            Type next = parser.next_stmt_type();
            if (next == CREATE) {
                Create create = parser.create_stmt();

                cout << "Until line: " << parser.getLine();
                cout << ", col " << parser.getCol() << "\n";

                cout << "create ID: " << create.getId() << '\n';

                cout << "create defaults" << '\n';
                const multimap<string, int> &defs = create.getDefaults();
                for (auto it = defs.begin(); it != defs.end(); ++it) {
                    cout << it->first << ": " << it->second << "\n";
                }

                cout << "create keys:\n";
                const vector<vector<string> > &keys = create.getKeys();
                for (auto key : keys) {
                    for (auto col: key) {
                        cout << col << "\n";
                    }
                }
                cout << "\n";
            } else if (next == INSERT) {
                Insert insert = parser.insert_stmt();

                cout << "Until line: " << parser.getLine();
                cout << ", col " << parser.getCol() << "\n";

                cout << "insert ID: " << insert.getId() << '\n';

                cout << "insert values" << '\n';
                const vector<string> &columns = insert.getColumns();
                const vector<int> &values = insert.getValues();
                for (int i = 0; i < columns.size(); ++i) {
                    cout << columns[i] << ": " << values[i] << '\n';
                }
                cout << "\n";
            } else if (next == DELETE) {
                Delete del = parser.delete_stmt();

                cout << "Until line: " << parser.getLine();
                cout << ", col " << parser.getCol() << "\n";

                cout << "delete ID: " << del.getId() << '\n';
                cout << "delete clause" << '\n';
                cout << del.getWhere();
                cout << "\n\n";
            } else if (next == SELECT) {
                Query query = parser.query_stmt();

                cout << "Until line: " << parser.getLine();
                cout << ", col " << parser.getCol() << "\n";

                cout << "query ID: " << query.getId() << '\n';

                cout << "query columns: ";
                const vector<string> &columns = query.getColumns();
                for (int i = 0; i < columns.size(); ++i) {
                    cout << columns[i] << ' ';
                }

                cout << "\nquery clause" << '\n';
                cout << query.getWhere();
                cout << "\n\n";
            }
        } catch (LexError e) {
            cout << lexptr->getLine() << ", " << lexptr->getCol() << ": ";
            cout << e.what()  << '\n';
        } catch (ParseError e) {
            cout << parser.getLine() << ", " << parser.getCol() << ": ";
            cout << e.what() << '\n';
        }
    }
    return 0;
}
