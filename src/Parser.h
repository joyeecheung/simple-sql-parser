#include "Lexer.h"
#include "Expr.h"
#include "Statements.h"

#ifndef PARSER_H
#define PARSER_H

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
    Expr conjunct_list();
    Expr _conjunct_list();
    Expr boolean();
    Expr operand();
    Type rop();

    Query query_stmt();
    Parser &select_list(vector<string> &columns);

    string id();
    int num();

    Parser &match(Type t);

    bool isEnd() {
        return lookahead == END;
    }

    ~Parser() {}

private:
    Token lookahead;
    Lexer &lexer;
};

#endif