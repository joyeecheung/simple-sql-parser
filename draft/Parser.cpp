// should do more than invoking functions
// todo: proper return type and parameters to build up 4 kinds of statements
// maybe one class for each statement, e.g.
// create statement ---- id
//                   |-- decl_list
//                          |--- default spec <string, int>(key, num)
//                          |--- primary keys vector
// so maybe create.id = id();
//            spec = map<string, int>; keys = vector<string>
//            decl_list(spec, keys)
//  then decl_list -> decl(spec, keys); _decl_list(spec, keys)  // by ref
//  the  decl -> ID => name = id(); num = default_spec(); spec[name] = num;
//            -> PRIMARY => keys.concatenate(column_list())


struct Create {
    string id;
    map<string, int> default_spec;
    vector<strings> keys;
};

struct Insert {
    string id;
    vector<string> columns,
    vector<int> values;
};

// construct a string key -> value table, pass to Expr to evaluate
struct Delete {
    string id;
    Expr where;
};

struct Query {
    string id;
    vector<string> select_list;  // can contain '*'
    Expr where;
};

class Parser {
public:
    Parser &ssql_stmt() {
        if (lookahead == CREATE) {
            create_stmt();
        } else if (lookahead == INSERT) {
            insert_stmt();
        } else if (lookahead == DELETE) {
            delete_stmt();
        } else if (lookahead == SELECT) {
            query_stmt();
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &create_stmt() {
        if (lookahead == CREATE) {
            match(CREATE); match(TABLE); id(); match(L_PAREN); decl_list(); match(R_PAREN) match(SEMICOLON);
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &decl_list() {
        if (lookahead == NUM || lookahead == ID) {
            decl(); _decl_list();
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &_decl_list() {
        if (lookahead == COMMA) {
            match(COMMA); decl(); _decl_list();
        } else if (lookahead == R_PAREN || lookahead == END){
            ;  // epsilon
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &decl() {
        if (lookahead == ID) {
            id(); match(INT); default_spec();
        } else if (lookahead == PRIMARY) {
            match(PRIMARY); match(KEY); match(L_PAREN); column_list(); match(R_PAREN);
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &default_spec() {
        if (lookahead == DEFAULT) {
            match(DEFAULT); match(EQ); num();
        } else if (lookahead == COMMA || lookahead == R_PAREN || lookahead == END) {
            ;
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &column_list() {
        if (lookahead == ID) {
            id(); _column_list();
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &_column_list() {
        if (lookahead == COMMA) {
            match(COMMA); id(); _column_list();
        } else if (lookahead == R_PAREN || lookahead == END){
            ;  // epsilon
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &insert_stmt() {
        if (lookahead == INSERT) {
            match(INSERT); match(TABLE); id(); match(L_PAREN); decl_list(); match(R_PAREN) match(SEMICOLON);
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &value_list() {
        if (lookahead == NUM) {
            num(); _value_list();
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &_value_list() {
        if (lookahead == COMMA) {
            match(COMMA); num(); _value_list();
        } else if (lookahead == R_PAREN || lookahead == END){
            ;  // epsilon
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &delete_stmt() {
        if (lookahead == DELETE) {
            match(DELETE); match(FROM); id(); where_clause(); match(SEMICOLON);
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &where_clause() {
        if (lookahead == WHERE) {
            match(WHERE); conjunct_list();
        } else if (lookahead == SEMICOLON || lookahead == END) {
            ;  // epsilon
        } else {
            throw ParseError("Syntax error");
        }

    }

    Parser &conjunct_list() {
        if (lookahead == NUM || lookahead == ID) {
            boolean(); _conjunct_list();
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &_conjunct_list() {
        if (lookahead == AND) {
            match(AND); boolean(); _conjunct_list();
        } else if (lookahead == SEMICOLON || lookahead == END) {
            ;  // epsilon
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &boolean() {
        if (lookahead == NUM || lookahead == ID) {
            operand(); rop(); operand();
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &operand() {
        if (lookahead == NUM) {
            num();
        } else if (lookahead == ID) {
            id();
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &rop() {
        if (lookahead == NEQ || lookahead == EQ
            || lookahead == LT || lookahead == GT
            || lookahead == LEQ || lookahead == GEQ) {
            match(lookahead.getType());
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &query_stmt() {
        if (lookahead == SELECT) {
            match(SELECT); select_list(); match(FROM); id(); where_clause(); match(SEMICOLON);
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &select_list() {
        if (lookahead == MUL) {
            match(MUL);
        } else {
            column_list();
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &id() {

    }

    Parser &num() {

    }

    Parser &match(Type t) {
        if (lookahead == t) {
            lookahead = lexer.next();
        } else {
            throw ParseError("Syntax error");
        }
    }
private:
    Token lookahead;
    Lexer &lexer;
};
