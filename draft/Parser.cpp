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

// class Statement {
// public:
//     Statement(string _id) : id(_id) {}
//     string getId() { return id; }
//     virtual void execute();
// private:
//     string id;
// };

// class Create : Statement {
// public:
//     Create(const string _id, const map<string, int> &_default_spec,
//            const vector<strings> &_keys) :
//             Statement(_id),
//             default_spec(_default_spec),
//             keys(_keys) {}

//     const map<string, int> &getDefaults() const {
//         return default_spec;
//     }

//     const vector<string> &getKeys() const {
//         return keys;
//     }

// private:
//     string id;
//     map<string, int> default_spec;
//     vector<string> keys;
// };

// class Insert : Statement {
// public:
//     Create(const string _id, const vector<string> &columns,
//            const vector<int> &values) :
//             Statement(_id),
//             default_spec(_default_spec),
//             keys(_keys) {}

//     const vector<string> &getColumns() const {
//         return columns;
//     }

//     const vector<ing> &getVales() const {
//         return values;
//     }

// private:
//     string id;
//     map<string, int> default_spec;
//     vector<string> keys;
// };

struct Create {
    Create(const string _id, const map<string, int> &_default_spec,
           const vector<string> &_keys)
        : id(_id), default_spec(_default_spec), keys(_keys) {}

    string id;
    map<string, int> default_spec;
    vector<string> keys;
};

struct Insert {
    Insert(const string _id, const vector<string> &_columns,
           const vector<int> &_values)
        : id(_id), columns(_columns), values(_values) {}

    string id;
    vector<string> columns,
    vector<int> values;
};

// construct a string key -> value table, pass to Expr to evaluate
struct Delete {
    Delete(const string _id, const Expr &_where)
        : id(_id), where(_where) {}

    string id;
    Expr where;
};

struct Query {
    Query(const string _id, const vector<string> _select_list,
          const Expr &_where)
        : id(_id), select_list(_select_list), where(_where) {}

    string id;
    vector<string> select_list;  // can contain '*'
    Expr where;
};

class Parser {
public:
    const Statement &ssql_stmt() {
        if (lookahead == CREATE) {
            return create_stmt();
        } else if (lookahead == INSERT) {
            return insert_stmt();
        } else if (lookahead == DELETE) {
            return delete_stmt();
        } else if (lookahead == SELECT) {
            return query_stmt();
        } else {
            throw ParseError("Syntax error");
        }
    }

    const Create &create_stmt() {
        if (lookahead == CREATE) {
            match(CREATE); match(TABLE);
            string table_id = id();
            match(L_PAREN);
            map<string, int> default_spec;
            vector<string> keys;
            decl_list(default_spec, keys);
            match(R_PAREN) match(SEMICOLON);
            return Create(id, default_spec, keys);
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

    const Insert &insert_stmt() {
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

    const Delete &delete_stmt() {
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

    const Query &query_stmt() {
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

    bool isEnd() {
        return lookahead == END;
    }

private:
    Token lookahead;
    Lexer &lexer;
};
