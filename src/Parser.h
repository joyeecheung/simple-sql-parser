#include "Lexer.h"
#include "Expr.h"
#include "Statements.h"

#ifndef PARSER_H
#define PARSER_H

namespace ssql {

class ParseError : std::exception {
public:
    ParseError(string _msg) : msg(_msg) {}
    ~ParseError() throw () {} // Updated
    const char *what() const throw() {
        return msg.c_str();
    }
private:
    string msg;
};

/* Predictive parser.
 *
 * Use RVO instead of move semantics for performance.
 * Usage:
 *     Parser parser(lexer);  // get a lexer for it first.
 *     // type of next statement. CREATE/DELETE/INSERT/SELECT
 *     Type next_type = parser.next_stmt_type();
 *     if (next_type == CREATE) {  // check the type first
 *         Create stmt = parser.create_stmt();  // get the statment
 *     } // and so forth...
 */
class Parser {
public:
    Parser(Lexer &l) : lexer(l) {
        col = lexer.getCol();
        line = lexer.getLine();
        lookahead = lexer.next();
        delay = false;
    }

    Type next_stmt_type();  // head of next statement

    Create create_stmt();
    Parser &decl_list(multimap<string, int> &defaults, vector<vector<string> > &keys);
    Parser &_decl_list(multimap<string, int> &defaults, vector<vector<string> > &keys);
    Parser &decl(multimap<string, int> &defaults, vector<vector<string> > &keys);
    int default_spec();
    Parser &column_list(vector<string> &names);
    Parser &_column_list(vector<string> &names);

    Insert insert_stmt();
    Parser &value_list(vector<int> &values);
    Parser &_value_list(vector<int> &values);

    Delete delete_stmt();
    // RVO is heavily used here
    Expr where_clause();
    Expr disjunct();
    Expr _disjunct();
    Expr conjunct();
    Expr _conjunct();
    Expr boolean();
    Expr comp();
    Expr expr(bool simple);
    Expr _expr(bool simple);
    Expr term(bool simple);
    Expr _term(bool simple);
    Expr unary(bool simple);

    Type rop();

    Query query_stmt();
    Parser &select_list(vector<string> &columns);

    // terminals
    string id();
    int num();
    Parser &cosume(Type t);  // cosume a keyword/operator

    // Use this to move the lookahead and record the col/line.
    // Don't call lexer.next() by hand.
    void advance();

    // next lookahead is END
    bool isEnd() const {
        return lookahead == END;
    }

    int getCol() const {
        return col;
    }

    int getLine() const {
        return line;
    }

    bool match(Type t) {
        if (delay) {
            advance();
        }
        return lookahead == t;
    }

    Parser &consume_until_start() {
        while (!match(CREATE) && !match(INSERT) && !match(DELETE)
               && !match(SELECT) && !match(END)) {
            advance();
        }
        return *this;
    }

    ~Parser() {}

private:
    Token lookahead;
    Lexer &lexer;
    int col;  // column number
    int line;  // line number
    bool delay;
};

}

#endif
