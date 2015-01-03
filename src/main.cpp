#include <iostream>
#include <fstream>
#include <iomanip>

#include "Token.h"
#include "Lexer.h"
#include "Parser.h"
#include "Engine.h"
#include "Expr.h"

using std::cin;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::setw;
using namespace ssql;

#define WIDTH_PER_COL 15

class IO {
public:
    IO(Parser &p, Lexer &l, Engine &e) : parser(p), lexer(l), engine(e) {}
    void printLine(int length, char ch = '-');
    void printData(const vector<string> &names,
                    vector<vector<int> > &results);
    void loop();
    void execute(Type next);
private:
    Parser &parser;
    Lexer &lexer;
    Engine &engine;
};

void IO::printLine(int length, char ch) {
    for (int i = 0; i < length; ++i) {
        cout << ch;
    }
    cout << '\n';
}

void IO::printData(const vector<string> &names,
                   vector<vector<int> > &results) {
    printLine(names.size() * WIDTH_PER_COL);
    for (const auto &name : names) {
        cout << "|" << setw(WIDTH_PER_COL - 1) << name;
    }
    cout << "|\n";
    printLine(names.size() * WIDTH_PER_COL);
    for (const auto &record : results) {
        for (auto col : record) {
            cout << "|" << setw(WIDTH_PER_COL - 1) << col;
        }
        cout << "|\n";
        printLine(names.size() * WIDTH_PER_COL);
    }
}

void IO::loop() {
    while (!parser.isEnd()) {
        try {
            Type next = parser.next_stmt_type();
            execute(next);
        } catch (LexError e) {
            cout << "line " << lexer.getLine() << ", ";
            cout << "column " << lexer.getCol() << ": ";
            cout << e.what()  << '\n';
        } catch (ParseError e) {
            cout << "line " << parser.getLine() << ", ";
            cout << "column " << parser.getCol() << ": ";
            cout << e.what() << '\n';
            parser.consume_until_start();
        } catch (DataBaseError e) {
            cout << "line " << parser.getLine() << ": ";
            cout << e.what()  << '\n';
        } catch (RuntimeError e) {
            cout << "line " << parser.getLine() << ", ";
            cout << e.what()  << '\n';
            parser.consume_until_start();
        }
    }
}

void IO::execute(Type next) {
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
        int number = engine.del(delete_stmt);
        cout << "Deleted " << number << " rows from table ";
        cout << delete_stmt.getId() << "\n";
    } else if (next == SELECT) {
        Query query_stmt = parser.query_stmt();
        string table_id = query_stmt.getId();

        vector<vector<int> > results;
        vector<string> names = query_stmt.getColumns();

        int number = engine.query(query_stmt, results);
        if (number > 0) {
            if (std::find(names.begin(), names.end(), "*")
                    != names.end()) {
                names = engine.getColumns(table_id);
            }

            printData(names, results);

            cout << number << " matching rows in ";
            cout << query_stmt.getId() << "\n";
        } else {
            cout << "No matching rows in ";
            cout << query_stmt.getId() << "\n";
        }
    } else if (next == END) {
        return;
    }
}

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