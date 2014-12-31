#include "Parser.h"

using std::pair;

Type Parser::next_stmt_type() const {
    if (lookahead == CREATE ||lookahead == INSERT
        || lookahead == DELETE || lookahead == SELECT) {
        return lookahead.getType();
    } else {
        throw ParseError("Syntax error: statments \
            should start with CREATE, INSERT, DELETE or SELECT");
    }
}

Create Parser::create_stmt() {
    if (lookahead == CREATE) {
        // create_stmt ->  CREATE TABLE id L_PAREN decl_list
        //                 R_PAREN SEMICOLON
        match(CREATE);
        match(TABLE);
        string table_id = id();
        match(L_PAREN);
        multimap<string, int> defaults;
        vector<vector<string> > keys;
        decl_list(defaults, keys);
        match(R_PAREN);
        match(SEMICOLON);
        return Create(table_id, defaults, keys);
    } else {
        throw ParseError("Syntax error");
    }
}

Parser & Parser::decl_list(multimap<string, int> &defaults, vector<vector<string> > &keys) {
    if (lookahead == ID || lookahead == PRIMARY) {
        // decl_list -> decl _decl_list
        decl(defaults, keys);
        _decl_list(defaults, keys);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Parser & Parser::_decl_list(multimap<string, int> &defaults, vector<vector<string> > &keys) {
    if (lookahead == COMMA) {
        // _decl_list -> COMMA decl _decl_list
        match(COMMA);
        decl(defaults, keys);
        _decl_list(defaults, keys);
    } else if (lookahead == R_PAREN) {
        ; // _decl_list -> epsilon
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Parser & Parser::decl(multimap<string, int> &defaults, vector<vector<string> > &keys) {
    if (lookahead == ID) {
        // decl -> id INT default_spec
        string name = id();
        match(INT);
        int number = default_spec();
        defaults.insert(std::pair<string,int>(name, number));
    } else if (lookahead == PRIMARY) {
        // decl -> PRIMARY KEY L_PAREN column_list R_PAREN
        match(PRIMARY);
        match(KEY);
        match(L_PAREN);
        vector<string> columns;
        column_list(columns);
        keys.push_back(columns);
        match(R_PAREN);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

int Parser::default_spec() {
    if (lookahead == DEFAULT) {
        // default_spec -> DEFAULT ASSIGN expr[true]
        match(DEFAULT);
        match(ASSIGN);
        Expr spec = expr(true);
        return spec.eval();
    } else if (lookahead == COMMA || lookahead == R_PAREN) {
        // default_spec -> epsilon
        return 0;  // if no default, default to zero
    } else {
        throw ParseError("Syntax error");
    }
}

Parser & Parser::column_list(vector<string> &names) {
    if (lookahead == ID) {
        // column_list ->  id _column_list
        names.push_back(id());
        _column_list(names);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Parser & Parser::_column_list(vector<string> &names) {
    if (lookahead == COMMA) {
        // _column_list ->  COMMA id _column_list
        match(COMMA);
        names.push_back(id());
        _column_list(names);
    } else if (lookahead == FROM || lookahead == R_PAREN){
        ;  // _column_list ->  epsilon
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Insert Parser::insert_stmt() {
    if (lookahead == INSERT) {
        // insert_stmt -> INSERT INTO id L_PAREN column_list R_PAREN
        //                VALUES L_PAREN value_list R_PAREN SEMICOLON
        match(INSERT);
        match(INTO);
        string table_id = id();
        match(L_PAREN);
        vector<string> columns;
        column_list(columns);
        match(R_PAREN);
        match(VALUES);
        match(L_PAREN);
        vector<int> values;
        value_list(values);
        match(R_PAREN);
        match(SEMICOLON);
        return Insert(table_id, columns, values);
    } else {
        throw ParseError("Syntax error");
    }
}

Parser & Parser::value_list(vector<int> &values) {
    if (lookahead == PLUS || lookahead == MINUS
        || lookahead == NUM || lookahead == L_PAREN) {
        // value_list -> expr[true] _value_list
        Expr value = expr(true);
        values.push_back(value.eval());
        _value_list(values);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Parser & Parser::_value_list(vector<int> &values) {
    if (lookahead == COMMA) {
        // _value_list -> COMMA expr[true] _value_list
        match(COMMA);
        Expr value = expr(true);
        values.push_back(value.eval());
        _value_list(values);
    } else if (lookahead == R_PAREN){
        ;  // _value_list ->  epsilon
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Delete Parser::delete_stmt() {
    if (lookahead == DELETE) {
        // delete_stmt -> DELETE FROM id where_clause SEMICOLON
        match(DELETE);
        match(FROM);
        string table_id = id();
        Expr where = where_clause();
        match(SEMICOLON);
        return Delete(table_id, where);
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::where_clause() {
    if (lookahead == WHERE) {
        // where_clause -> WHERE disjunct
        match(WHERE);
        return disjunct();
    } else if (lookahead == SEMICOLON) {
        return NULL_EXPR;  // where_clause -> epsilon
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::disjunct() {
    if (lookahead == L_PAREN || lookahead == NOT
        || lookahead == PLUS || lookahead == MINUS
        || lookahead == NUM || lookahead == ID) {
        // disjunct -> conjunct _disjunct
        Expr temp = conjunct();
        Expr test = _disjunct();
        if (test.isNull()) {  // test lack a left node
            return temp;
        } else {
            test.setLeft(temp);
            return test;
        }
    } else {
        throw ParseError("Syntax error");
    }
}


Expr Parser::_disjunct() {
    if (lookahead == OR) {
        // _disjunct -> OR conjunct _disjunct
        match(OR);
        Expr root(OR);

        Expr temp = conjunct();
        Expr test = _disjunct();
        if (test.isNull()) {
            root.setRight(temp);
            return root;
        } else {
            test.setLeft(temp);
            root.setRight(test);
            return root;
        }
    } else if (lookahead == SEMICOLON || lookahead == R_PAREN) {
        return NULL_EXPR;  // _disjunct-> epsilon
    } else {
        throw ParseError("Syntax error");
    }
}


Expr Parser::conjunct() {
    if (lookahead == L_PAREN || lookahead == NOT
        || lookahead == PLUS || lookahead == MINUS
        || lookahead == NUM || lookahead == ID) {
        // conjunct -> bool _conjunct_list
        Expr temp = boolean();
        Expr test = _conjunct();
        if (test.isNull()) {  // test lack a left node
            return temp;
        } else {
            test.setLeft(temp);
            return test;
        }
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::_conjunct() {
    if (lookahead == AND) {
        // _conjunct -> AND bool _conjunct
        match(AND);
        Expr root(AND);

        Expr temp = boolean();
        Expr test = _conjunct();
        if (test.isNull()) {
            root.setRight(temp);
            return root;
        } else {
            test.setLeft(temp);
            root.setRight(test);
            return root;
        }
    } else if (lookahead == OR || lookahead == SEMICOLON
            || lookahead == R_PAREN) {
        return NULL_EXPR;  // _conjunct -> epsilon
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::boolean() {
    if (lookahead == NUM || lookahead == ID
     || lookahead == PLUS || lookahead == MINUS) {
        return comp();
    } else if (lookahead == L_PAREN) {
        match(L_PAREN);
        Expr temp = disjunct();
        match(R_PAREN);
        return temp;
    } else if (lookahead == NOT) {
        match(NOT);
        Expr temp(NOT);
        temp.setRight(disjunct());
        return temp;
    } else {
        throw ParseError("Syntax error");
    }
}


Expr Parser::comp() {
    if (lookahead == NUM || lookahead == ID
     || lookahead == PLUS || lookahead == MINUS) {
        Expr temp;
        temp.setLeft(expr(false));
        temp.setType(rop());
        temp.setRight(expr(false));
        return temp;
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::expr(bool simple) {
    if (lookahead == NUM || lookahead == PLUS || lookahead == MINUS
        || (simple && lookahead == L_PAREN) || (!simple && lookahead == ID)) {
        // expr[] -> term[] _expr[]
        Expr temp = term(simple);
        Expr test = _expr(simple);
        if (test.isNull()) {  // test lack a left node
            return temp;
        } else {
            test.setLeft(temp);
            return test;
        }
        return temp;
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::_expr(bool simple) {
    if (lookahead == PLUS || lookahead == MINUS) {
        // _expr[] -> PLUS term[] _expr[]
        // _expr[] -> MINUS term[] _expr[]
        Type op = lookahead.getType();
        match(op);
        Expr root(op);

        Expr temp = term(simple);
        Expr test = _expr(simple);
        if (test.isNull()) {
            root.setRight(temp);
            return root;
        } else {
            test.setLeft(temp);
            root.setRight(test);
            return root;
        }
    } else if (simple && (lookahead == COMMA || lookahead == R_PAREN)) {
        return NULL_EXPR;  // _expr[] -> epsilon
    } else if (!simple && (lookahead == NEQ || lookahead == EQ
        || lookahead == LT || lookahead == GT || lookahead == LEQ
        || lookahead == GEQ || lookahead == AND || lookahead == OR
        || lookahead == SEMICOLON || lookahead == R_PAREN)) {
        return NULL_EXPR;  // _expr[] -> epsilon
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::term(bool simple) {
    if (lookahead == NUM || lookahead == PLUS || lookahead == MINUS
        || (simple && lookahead == L_PAREN) || (!simple && lookahead == ID)) {
        // term[] -> unary[] _term[]
        Expr temp = unary(simple);
        Expr test = _term(simple);
        if (test.isNull()) {  // test lack a left node
            return temp;
        } else {
            test.setLeft(temp);
            return test;
        }
        return temp;
    } else {
        throw ParseError("Syntax error");
    }
}


Expr Parser::_term(bool simple) {
    if (lookahead == MUL || lookahead == DIV) {
        // _term[] -> MUL unary[] _term[]
        // _term[] -> DIV unary[] _term[]
        Type op = lookahead.getType();
        match(op);
        Expr root(op);

        Expr temp = term(simple);
        Expr test = _expr(simple);
        if (test.isNull()) {
            root.setRight(temp);
            return root;
        } else {
            test.setLeft(temp);
            root.setRight(test);
            return root;
        }
    } else if (simple && (lookahead == COMMA || lookahead == R_PAREN)) {
        return NULL_EXPR;  // _term[] -> epsilon
    } else if (!simple && (lookahead == NEQ || lookahead == EQ
        || lookahead == LT || lookahead == GT || lookahead == LEQ
        || lookahead == GEQ || lookahead == AND || lookahead == OR
        || lookahead == SEMICOLON || lookahead == R_PAREN)) {
        return NULL_EXPR;  // _term[]-> epsilon
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::unary(bool simple) {
    if (lookahead == PLUS || lookahead == MINUS) {
        // unary[] -> PLUS unary[]
        // unary[] -> MINUS unary[]
        Type op = lookahead.getType();
        match(op);
        Expr root(op);
        root.setRight(unary(simple));
        return root;
    } else if (lookahead == NUM) {
        // unary[] ->  num
        int result = num();
        Expr temp(NUM);
        temp.setValue(Token(NUM, &result, sizeof(result)));
        return temp;
    } else if (!simple && lookahead == ID) {
        // unary[false] -> id
        string result = id();
        Expr temp(ID);
        temp.setValue(Token(ID, result.c_str(), result.size()));
        return temp;
    else if (simple && lookahead == L_PAREN) {
        // unary[true] -> L_PAREN expr[true] R_PAREN
        match(L_PAREN);
        Expr temp = expr(true);
        match(R_PAREN);
        return temp;
    } else {
        throw ParseError("Syntax error");
    }
}

Type Parser::rop() {
    if (lookahead == NEQ || lookahead == EQ
        || lookahead == LT || lookahead == GT
        || lookahead == LEQ || lookahead == GEQ) {
        Type result = lookahead.getType();
        match(result);
        return result;
    } else {
        throw ParseError("Syntax error");
    }
}

Query Parser::query_stmt() {
    if (lookahead == SELECT) {
        // query_stmt -> SELECT select_list FROM id where_clause SEMICOLON
        match(SELECT);
        vector<string> columns;
        select_list(columns);
        match(FROM);
        string table_id = id();
        Expr where = where_clause();
        match(SEMICOLON);
        return Query(table_id, columns, where);
    } else {
        throw ParseError("Syntax error");
    }
}

Parser & Parser::select_list(vector<string> &columns) {
    if (lookahead == MUL) {
        // select_list -> MUL
        columns.push_back("*");
        match(MUL);
    } else if (lookahead == ID) {
        // select_list -> column_list
        column_list(columns);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

string Parser::id() {
    if (lookahead == ID) {
        string result = lookahead.getId();
        advance();
        return result;
    } else {
        throw ParseError("Syntax error: Expected ID");
    }
}

int Parser::num() {
    if (lookahead == NUM) {
        int result = lookahead.getNumber();
        advance();
        return result;
    } else {
        throw ParseError("Syntax error: Expected number");
    }
}

Parser & Parser::match(Type t) {
    if (lookahead == t) {
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
    col = lexer.getCol();
    line = lexer.getLine();
    lookahead = lexer.next();
}
