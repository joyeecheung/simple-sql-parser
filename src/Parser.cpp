#include "Parser.h"

namespace ssql {

using std::pair;

Type Parser::next_stmt_type() {
    if (match(CREATE) || match(INSERT)
            || match(DELETE) || match(SELECT)) {
        return lookahead.getType();
    } else if (match(END)) {
        return END;
    } else {
        throw ParseError(string("Syntax error: statments should start with")
                         + string(" CREATE, INSERT, DELETE or SELECT"));
    }
}

Create Parser::create_stmt() {
    if (match(CREATE)) {
        // create_stmt ->  CREATE TABLE id L_PAREN decl_list
        //                 R_PAREN SEMICOLON
        cosume(CREATE);
        cosume(TABLE);
        string table_id = id();
        cosume(L_PAREN);
        multimap<string, int> defaults;
        vector<vector<string> > keys;
        decl_list(defaults, keys);
        cosume(R_PAREN);
        cosume(SEMICOLON);
        return Create(table_id, defaults, keys);
    } else {
        throw ParseError("Syntax error");
    }
}

Parser &Parser::decl_list(multimap<string, int> &defaults,
                          vector<vector<string> > &keys) {
    if (match(ID) || match(PRIMARY)) {
        // decl_list -> decl _decl_list
        decl(defaults, keys);
        _decl_list(defaults, keys);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Parser &Parser::_decl_list(multimap<string, int> &defaults,
                           vector<vector<string> > &keys) {
    if (match(COMMA)) {
        // _decl_list -> COMMA decl _decl_list
        cosume(COMMA);
        decl(defaults, keys);
        _decl_list(defaults, keys);
    } else if (match(R_PAREN)) {
        ; // _decl_list -> epsilon
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Parser &Parser::decl(multimap<string, int> &defaults,
                     vector<vector<string> > &keys) {
    if (match(ID)) {
        // decl -> id INT default_spec
        string name = id();
        cosume(INT);
        int number = default_spec();
        defaults.insert(std::pair<string, int>(name, number));
    } else if (match(PRIMARY)) {
        // decl -> PRIMARY KEY L_PAREN column_list R_PAREN
        cosume(PRIMARY);
        cosume(KEY);
        cosume(L_PAREN);
        vector<string> columns;
        column_list(columns);
        keys.push_back(columns);
        cosume(R_PAREN);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

int Parser::default_spec() {
    if (match(DEFAULT)) {
        // default_spec -> DEFAULT ASSIGN expr[true]
        cosume(DEFAULT);
        cosume(ASSIGN);
        Expr spec = expr(true);
#ifdef TRACK
        std::cout << '\n' << spec << '\n';
#endif
        return spec.eval();
    } else if (match(COMMA) || match(R_PAREN)) {
        // default_spec -> epsilon
        return 0;  // if no default, default to zero
    } else {
        throw ParseError("Syntax error");
    }
}

Parser &Parser::column_list(vector<string> &names) {
    if (match(ID)) {
        // column_list ->  id _column_list
        names.push_back(id());
        _column_list(names);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Parser &Parser::_column_list(vector<string> &names) {
    if (match(COMMA)) {
        // _column_list ->  COMMA id _column_list
        cosume(COMMA);
        names.push_back(id());
        _column_list(names);
    } else if (match(FROM) || match(R_PAREN)) {
        ;  // _column_list ->  epsilon
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Insert Parser::insert_stmt() {
    if (match(INSERT)) {
        // insert_stmt -> INSERT INTO id L_PAREN column_list R_PAREN
        //                VALUES L_PAREN value_list R_PAREN SEMICOLON
        cosume(INSERT);
        cosume(INTO);
        string table_id = id();
        cosume(L_PAREN);
        vector<string> columns;
        column_list(columns);
        cosume(R_PAREN);
        cosume(VALUES);
        cosume(L_PAREN);
        vector<int> values;
        value_list(values);
        cosume(R_PAREN);
        cosume(SEMICOLON);
        return Insert(table_id, columns, values);
    } else {
        throw ParseError("Syntax error");
    }
}

Parser &Parser::value_list(vector<int> &values) {
    if (match(PLUS) || match(MINUS)
            || match(NUM) || match(L_PAREN)) {
        // value_list -> expr[true] _value_list
        Expr value = expr(true);
#ifdef TRACK
        std::cout << '\n' << value << '\n';
#endif
        values.push_back(value.eval());
        _value_list(values);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Parser &Parser::_value_list(vector<int> &values) {
    if (match(COMMA)) {
        // _value_list -> COMMA expr[true] _value_list
        cosume(COMMA);
        Expr value = expr(true);
#ifdef TRACK
        std::cout << '\n' << value << '\n';
#endif
        values.push_back(value.eval());
        _value_list(values);
    } else if (match(R_PAREN)) {
        ;  // _value_list ->  epsilon
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Delete Parser::delete_stmt() {
    if (match(DELETE)) {
        // delete_stmt -> DELETE FROM id where_clause SEMICOLON
        cosume(DELETE);
        cosume(FROM);
        string table_id = id();
        Expr where = where_clause();
        cosume(SEMICOLON);
        return Delete(table_id, where);
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::where_clause() {
    if (match(WHERE)) {
        // where_clause -> WHERE disjunct
        cosume(WHERE);
        return disjunct();
    } else if (match(SEMICOLON)) {
        return NULL_EXPR;  // where_clause -> epsilon
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::disjunct() {
    if (match(L_PAREN) || match(NOT)
            || match(PLUS) || match(MINUS)
            || match(NUM) || match(ID)) {
        // disjunct -> conjunct _disjunct
        Expr temp = conjunct();
        Expr test = _disjunct();
        if (test.isNull()) {  // test lack a left node
            return temp;
        } else {
            test.setLeftMost(temp);
            return test;
        }
    } else {
        throw ParseError("Syntax error when maching disjunct");
    }
}


Expr Parser::_disjunct() {
    if (match(OR)) {
        // _disjunct -> OR conjunct _disjunct
        cosume(OR);
        Expr root(OR);

        Expr temp = conjunct();
        Expr test = _disjunct();
        if (test.isNull()) {
            root.setRight(temp);
            return root;
        } else {
            root.setRight(temp);
            test.setLeft(root);
            return test;
        }
    } else if (match(SEMICOLON) || match(R_PAREN)) {
        return NULL_EXPR;  // _disjunct-> epsilon
    } else {
        throw ParseError("Syntax error when matching disjunct");
    }
}


Expr Parser::conjunct() {
    if (match(L_PAREN) || match(NOT)
            || match(PLUS) || match(MINUS)
            || match(NUM) || match(ID)) {
        // conjunct -> bool _conjunct
        Expr temp = boolean();
        Expr test = _conjunct();
        if (test.isNull()) {  // test lack a left node
            return temp;
        } else {
            test.setLeftMost(temp);
            return test;
        }
    } else {
        throw ParseError("Syntax error when matching conjunct");
    }
}

Expr Parser::_conjunct() {
    if (match(AND)) {
        // _conjunct -> AND bool _conjunct
        cosume(AND);
        Expr root(AND);

        Expr temp = boolean();
        Expr test = _conjunct();

        if (test.isNull()) {
            root.setRight(temp);
            return root;
        } else {
            root.setRight(temp);
            test.setLeft(root);
            return test;
        }
    } else if (match(OR) || match(SEMICOLON)
               || match(R_PAREN)) {
        return NULL_EXPR;  // _conjunct -> epsilon
    } else {
        throw ParseError("Syntax error when matching conjunct");
    }
}

Expr Parser::boolean() {
    if (match(NUM) || match(ID)
            || match(PLUS) || match(MINUS)) {
        // bool -> comp
        return comp();
    } else if (match(L_PAREN)) {
        // bool -> L_PAREN disjunct R_PAREN
        cosume(L_PAREN);
        Expr temp = disjunct();
        cosume(R_PAREN);
        return temp;
    } else if (match(NOT)) {
        // bool -> NOT bool
        cosume(NOT);
        Expr temp(NOT);
        temp.setRight(boolean());
        return temp;
    } else {
        throw ParseError("Syntax error when matching boolean");
    }
}


Expr Parser::comp() {
    if (match(NUM) || match(ID)
            || match(PLUS) || match(MINUS)) {
        Expr temp;
        temp.setLeft(expr(false));
        temp.setType(rop());
        temp.setRight(expr(false));
        return temp;
    } else {
        throw ParseError("Syntax error when maching comparisons");
    }
}

Expr Parser::expr(bool simple) {
    if (match(NUM) || match(PLUS) || match(MINUS)
            || (simple && match(L_PAREN)) || (!simple && match(ID))) {
        // expr[] -> term[] _expr[]
        Expr temp = term(simple);
        Expr test = _expr(simple);
        if (test.isNull()) {  // test lack a left node
            return temp;
        } else {
            test.setLeftMost(temp);
            return test;
        }
    } else {
        throw ParseError("Syntax error when matching expressions");
    }
}

Expr Parser::_expr(bool simple) {
    if (match(PLUS) || match(MINUS)) {
        // _expr[] -> PLUS term[] _expr[]
        // _expr[] -> MINUS term[] _expr[]
        Type op = lookahead.getType();
        cosume(op);
        Expr root(op);

        Expr temp = term(simple);
        Expr test = _expr(simple);
        if (test.isNull()) {
            root.setRight(temp);
            return root;
        } else {
            root.setRight(temp);
            test.setLeft(root);
            return test;
        }
    } else if (simple && (match(COMMA) || match(R_PAREN))) {
        return NULL_EXPR;  // _expr[] -> epsilon
    } else if (!simple && (match(NEQ) || match(EQ)
                           || match(LT) || match(GT) || match(LEQ)
                           || match(GEQ) || match(AND) || match(OR)
                           || match(SEMICOLON) || match(R_PAREN))) {
        return NULL_EXPR;  // _expr[] -> epsilon
    } else {
        throw ParseError("Syntax error when matching expressions");
    }
}

Expr Parser::term(bool simple) {
    if (match(NUM) || match(PLUS) || match(MINUS)
            || (simple && match(L_PAREN)) || (!simple && match(ID))) {
        // term[] -> unary[] _term[]
        Expr temp = unary(simple);
        Expr test = _term(simple);
        if (test.isNull()) {  // test lack a left node
            return temp;
        } else {
            test.setLeftMost(temp);
            return test;
        }
    } else {
        throw ParseError("Syntax error when matching terms");
    }
}


Expr Parser::_term(bool simple) {
    if (match(MUL) || match(DIV)) {
        // _term[] -> MUL unary[] _term[]
        // _term[] -> DIV unary[] _term[]
        Type op = lookahead.getType();
        cosume(op);
        Expr root(op);

        Expr temp = unary(simple);
        Expr test = _term(simple);
        if (test.isNull()) {
            root.setRight(temp);
            return root;
        } else {
            root.setRight(temp);
            test.setLeft(root);
            return test;
        }
    } else if (match(PLUS) || match(MINUS)) {
        return NULL_EXPR;  // _term[] -> epsilon
    } else if (simple && (match(COMMA) || match(R_PAREN))) {
        return NULL_EXPR;  // _term[] -> epsilon
    } else if (!simple && (match(NEQ) || match(EQ)
                           || match(LT) || match(GT) || match(LEQ)
                           || match(GEQ) || match(AND) || match(OR)
                           || match(SEMICOLON) || match(R_PAREN))) {
        return NULL_EXPR;  // _term[]-> epsilon
    } else {
        throw ParseError("Syntax error when matching terms");
    }
}

Expr Parser::unary(bool simple) {
    if (match(PLUS) || match(MINUS)) {
        // unary[] -> PLUS unary[]
        // unary[] -> MINUS unary[]
        Type op = lookahead.getType();
        cosume(op);
        Expr root(op);
        root.setRight(unary(simple));
        return root;
    } else if (match(NUM)) {
        // unary[] ->  num
        int result = num();
        Expr temp(NUM);
        temp.setValue(Token(NUM, &result, sizeof(result)));
        return temp;
    } else if (!simple && match(ID)) {
        // unary[false] -> id
        string result = id();
        Expr temp(ID);
        temp.setValue(Token(ID, result.c_str(), result.size()));
        return temp;
    } else if (simple && match(L_PAREN)) {
        // unary[true] -> L_PAREN expr[true] R_PAREN
        cosume(L_PAREN);
        Expr temp = expr(true);
        cosume(R_PAREN);
        return temp;
    } else {
        throw ParseError("Syntax error when matching unary");
    }
}

Type Parser::rop() {
    if (match(NEQ) || match(EQ)
            || match(LT) || match(GT)
            || match(LEQ) || match(GEQ)) {
        Type result = lookahead.getType();
        cosume(result);
        return result;
    } else {
        throw ParseError("Syntax error when matching relational operators");
    }
}

Query Parser::query_stmt() {
    if (match(SELECT)) {
        // query_stmt -> SELECT select_list FROM id where_clause SEMICOLON
        cosume(SELECT);
        vector<string> columns;
        select_list(columns);
        cosume(FROM);
        string table_id = id();
        Expr where = where_clause();
        cosume(SEMICOLON);
        return Query(table_id, columns, where);
    } else {
        throw ParseError("Syntax error");
    }
}

Parser &Parser::select_list(vector<string> &columns) {
    if (match(MUL)) {
        // select_list -> MUL
        columns.push_back("*");
        cosume(MUL);
    } else if (match(ID)) {
        // select_list -> column_list
        column_list(columns);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

string Parser::id() {
    if (match(ID)) {
        string result = lookahead.getId();
        advance();
        return result;
    } else {
        throw ParseError("Syntax error: Expected ID");
    }
}

int Parser::num() {
    if (match(NUM)) {
        int result = lookahead.getNumber();
        advance();
        return result;
    } else {
        throw ParseError("Syntax error: Expected number");
    }
}

Parser &Parser::cosume(Type t) {
    if (match(t)) {
        advance();
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

void Parser::advance() {
#ifdef TRACK
    std::cout << lookahead << ' ';
    if (lookahead == SEMICOLON)
        std::cout << '\n';
#endif
    if (lookahead == SEMICOLON && !delay) {
        delay = true;
    } else {
        col = lexer.getCol();
        line = lexer.getLine();
        lookahead = lexer.next();
        delay = false;
    }
}

}