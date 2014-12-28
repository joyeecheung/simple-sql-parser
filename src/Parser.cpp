#include "Parser.h"


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
        match(CREATE); match(TABLE);
        string table_id = id();
        match(L_PAREN);
        map<string, int> defaults;
        vector<string> keys;
        decl_list(defaults, keys);
        match(R_PAREN); match(SEMICOLON);
        return Create(table_id, defaults, keys);
    } else {
        throw ParseError("Syntax error");
    }
}

Parser & Parser::decl_list(map<string, int> &defaults, vector<string> &keys) {
    if (lookahead == NUM || lookahead == ID) {
        // decl_list -> decl _decl_list
        decl(defaults, keys);
        _decl_list(defaults, keys);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Parser & Parser::_decl_list(map<string, int> &defaults, vector<string> &keys) {
    if (lookahead == COMMA) {
        // _decl_list -> COMMA decl _decl_list
        match(COMMA); decl(defaults, keys);
        _decl_list(defaults, keys);
    } else if (lookahead == R_PAREN || lookahead == END) {
        ; // _decl_list -> epsilon
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Parser & Parser::decl(map<string, int> &defaults, vector<string> &keys) {
    if (lookahead == ID) {
        // decl -> id INT default_spec
        string name = id();
        match(INT);
        int num = default_spec(defaults);
        defaults[name] = num;
    } else if (lookahead == PRIMARY) {
        // decl -> PRIMARY KEY L_PAREN column_list R_PAREN
        match(PRIMARY); match(KEY); match(L_PAREN);
        column_list(keys);
        match(R_PAREN);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

int Parser::default_spec(map<string, int> &defaults) {
    if (lookahead == DEFAULT) {
        // default_spec -> DEFAULT ASSIGN num
        match(DEFAULT); match(ASSIGN);
        return num();
    } else if (lookahead == COMMA || lookahead == R_PAREN || lookahead == END) {
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
    } else if (lookahead == FROM || lookahead == R_PAREN || lookahead == END){
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
        match(INSERT); match(INTO);
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
    if (lookahead == NUM) {
        // value_list ->  num _value_list
        values.push_back(num());
        _value_list(values);
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Parser & Parser::_value_list(vector<int> &values) {
    if (lookahead == COMMA) {
        // _value_list ->  COMMA num _value_list
        match(COMMA);
        values.push_back(num());
        _value_list(values);
    } else if (lookahead == R_PAREN || lookahead == END){
        ;  // _value_list ->  epsilon
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}

Delete Parser::delete_stmt() {
    if (lookahead == DELETE) {
        // delete_stmt -> DELETE FROM id where_clause SEMICOLON
        match(DELETE); match(FROM);
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
        // where_clause -> WHERE conjunct_list
        match(WHERE);
        return conjunct_list();
    } else if (lookahead == SEMICOLON || lookahead == END) {
        return NULL_EXPR;  // where_clause -> epsilon
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::conjunct_list() {
    if (lookahead == NUM || lookahead == ID) {
        // conjunct_list -> bool _conjunct_list
        Expr temp = boolean();
        Expr test = _conjunct_list();
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

Expr Parser::_conjunct_list() {
    if (lookahead == AND) {
        // _conjunct_list -> AND bool _conjunct_list
        match(AND);
        Expr root(AND);

        Expr temp = boolean();
        Expr test = _conjunct_list();
        if (test.isNull()) {
            root.setRight(temp);
            return root;
        } else {
            test.setLeft(temp);
            root.setRight(test);
            return root;
        }
    } else if (lookahead == SEMICOLON || lookahead == END) {
        return NULL_EXPR;
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::boolean() {
    if (lookahead == NUM || lookahead == ID) {
        Expr temp;
        temp.setLeft(operand());
        temp.setType(rop());
        temp.setRight(operand());
        return temp;
    } else {
        throw ParseError("Syntax error");
    }
}

Expr Parser::operand() {
    if (lookahead == NUM) {
        int result = num();
        Expr temp(NUM);
        temp.setValue(Token(NUM, &result, sizeof(result)));
        return temp;
    } else if (lookahead == ID) {
        string result = id();
        Expr temp(ID);
        temp.setValue(Token(ID, result.c_str(), result.size()));
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
        vector<string> columns;
        match(SELECT);
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
#ifdef TRACK
        std::cout << lookahead << ' ';
#endif
        string result = lookahead.getId();
        lookahead = lexer.next();
        return result;
    } else {
        throw ParseError("Syntax error: Expected ID");
    }
}

int Parser::num() {
    if (lookahead == NUM) {
#ifdef TRACK
        std::cout << lookahead << ' ';
#endif
        int result = lookahead.getNumber();
        lookahead = lexer.next();
        return result;
    } else {
        throw ParseError("Syntax error: Expected number");
    }
}

Parser & Parser::match(Type t) {
    if (lookahead == t) {
#ifdef TRACK
        std::cout << lookahead << ' ';
        if (lookahead == SEMICOLON)
            std::cout << '\n';
#endif
        lookahead = lexer.next();
    } else {
        throw ParseError("Syntax error");
    }
    return *this;
}