#include <iostream>
#include <fstream>

#include "Token.h"
#include "Lexer.h"
#include "Parser.h"
#include "Engine.h"

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

    Parser parser(*lexptr);
    Engine engine;

    while (!parser.isEnd()) {
        try {
            Type next = parser.next_stmt_type();
            if (next == CREATE) {
                Create create_stmt = parser.create_stmt();
                engine.create(create_stmt);
                cout << "Created table " << create_stmt.getId() << "\n";
            } else if (next == INSERT) {
                Insert insert_stmt = parser.insert_stmt();
                int number = engine.insert(insert_stmt);
                cout << "Inserted " << number << " rows into table ";
                cout << insert_stmt.getId() << "\n";
            } else if (next == DELETE) {
                Delete delete_stmt = parser.delete_stmt();
                int number = engine.del(insert_stmt);
                cout << "Deleted " << number << " rows from table ";
                cout << delete_stmt.getId() << "\n";
            } else if (next == SELECT) {
                Query query_stmt = parser.query_stmt();
                string table_id = query_stmt.getId();

                vector<vector<int> > results;
                vector<string> names = query_stmt.getColumns();

                int number = engine.query(query_stmt, results);
                if (number > 0) {
                    if (std::find(name.begin(), names.end(), "*")
                        != names.end()) {
                        names = engine.getColumns(table_id);
                    }

                    for (auto name : names) {
                        cout << name << '\t';
                    }
                    cout << '\n';

                    for (auto record : results) {
                        for (auto col : record) {
                            cout << col << '\t'
                        }
                        cout << '\n';
                    }
                    cout << number <<< " matching rows in ";
                    cout << query_stmt.getId() << "\n";
                } else {
                    cout << "No matching rows in ";
                    cout << query_stmt.getId() << "\n";
                }
            }
        } catch (LexError e) {
            cout << lexptr->getLine() << ": " << e.what()  << '\n';
        } catch (ParseError e) {
            cout << lexptr->getLine() << ": " << e.what() << '\n';
        } catch(DataBaseError e) {
            cout << lexptr->getLine() << ": " << e.what()  << '\n';
        }
    }

    delete lexptr;
    return 0;
}