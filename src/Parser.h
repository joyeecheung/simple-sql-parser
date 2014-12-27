#include "Lexer.h"
#include "Expr.h"

#ifndef PARSER_H
#define PARSER_H

class Statement {
public:
    Statement(string _id) : id(_id) {}
    string getId() { return id; }
private:
    string id;
};

class Create : public Statement {
public:
    Create(const string _id, const map<string, int> &_default_spec,
           const vector<string> &_keys) :
            Statement(_id),
            default_spec(_default_spec),
            keys(_keys) {}

    const map<string, int> &getDefaults() const {
        return default_spec;
    }

    const vector<string> &getKeys() const {
        return keys;
    }

private:
    string id;
    map<string, int> default_spec;
    vector<string> keys;
};

class Insert : public Statement {
public:
    Insert(const string _id, const vector<string> &_columns,
           const vector<int> &_values) :
            Statement(_id),
            columns(_columns),
            values(_values) {}

    const vector<string> &getColumns() const {
        return columns;
    }

    const vector<int> &getVales() const {
        return values;
    }

private:
    string id;
    vector<string> columns;
    vector<int> values;
};

class Delete : public Statement {
public:
    Delete(const string _id, const Expr &_where)
        : Statement(_id), where(_where) {}
    const Expr &getWhere() const {
        return where;
    }
private:
    string id;
    Expr where;
};

class Query : public Statement {
public:
    Query(const string _id, const vector<string> _columns,
          const Expr &_where)
        : Statement(_id), columns(_columns), where(_where) {}
    const vector<string> &getColumns() const {
        return columns;
    }
    const Expr &getWhere() const {
        return where;
    }
private:
    string id;
    vector<string> columns;  // can contain '*'
    Expr where;
};

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
    Parser(Lexer &l) : lexer(l), current_stmt(NULL) {}
    const Statement *ssql_stmt();

    Create *create_stmt();
    Parser &decl_list(map<string, int> &defaults, vector<string> &keys);
    Parser &_decl_list(map<string, int> &defaults, vector<string> &keys);
    Parser &decl(map<string, int> &defaults, vector<string> &keys);
    int default_spec(map<string, int> &defaults);
    Parser &column_list(vector<string> &names);
    Parser &_column_list(vector<string> &names);

    Insert *insert_stmt();
    Parser &value_list(vector<int> values);
    Parser &_value_list(vector<int> values);

    Delete *delete_stmt();
    Expr where_clause();
    Expr conjunct_list();
    Expr _conjunct_list();
    Expr boolean();
    Expr operand();
    Type rop();

    Query *query_stmt();
    Parser &select_list(vector<string> &columns);

    string id();
    int num();

    Parser &match(Type t);

    bool isEnd() {
        return lookahead == END;
    }

    ~Parser() {
        if (current_stmt != NULL) {
            delete current_stmt;
        }
    }

private:
    Token lookahead;
    Lexer &lexer;
    Statement *current_stmt;
};

#endif