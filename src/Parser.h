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


class Parser {
public:
    Parser(Lexer &l) : lexer(l) {
        col = lexer.getCol();
        line = lexer.getLine();
        lookahead = lexer.next();
    }
    Type next_stmt_type() const;

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

    string id();
    int num();

    Parser &match(Type t);
    void advance();

    bool isEnd() const {
        return lookahead == END;
    }

    int getCol() const {
        return col;
    }

    int getLine() const {
        return line;
    }

    ~Parser() {}

private:
    Token lookahead;
    Lexer &lexer;
    int col;  // column number
    int line;  // line number
};

}

#endif
