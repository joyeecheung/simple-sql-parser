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

class Statement {
public:
    Statement(string _id) : id(_id) {}
    string getId() { return id; }
    virtual void execute();
private:
    string id;
};

class Create : Statement {
public:
    Create(const string _id, const map<string, int> &_default_spec,
           const vector<strings> &_keys) :
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

class Insert : Statement {
public:
    Create(const string _id, const vector<string> &columns,
           const vector<int> &values) :
            Statement(_id),
            default_spec(_default_spec),
            keys(_keys) {}

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

class Delete : Statement {
public:
    Delete(const string _id, const Expr &_where)
        : id(_id), where(_where) {}
    const Expr &getWhere() const {
        return where;
    }
private:
    string id;
    Expr where;
};

class Query : Statement {
public:
    Query(const string _id, const vector<string> _columns,
          const Expr &_where)
        : id(_id), columns(_columns), where(_where) {}
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
            // create_stmt ->  CREATE TABLE id L_PAREN decl_list
            //                 R_PAREN SEMICOLON
            match(CREATE); match(TABLE);
            string table_id = id();
            match(L_PAREN);
            map<string, int> defaults;
            vector<string> keys;
            decl_list(defaults, keys);
            match(R_PAREN) match(SEMICOLON);
            return Create(id, defaults, keys);
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &decl_list(map<string, int> &defaults, vector<string> &keys) {
        if (lookahead == NUM || lookahead == ID) {
            // decl_list -> decl _decl_list
            decl(defaults, keys);
            _decl_list(defaults, keys);
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &_decl_list(map<string, int> &defaults, vector<string> &keys) {
        if (lookahead == COMMA) {
            // _decl_list -> COMMA decl _decl_list
            match(COMMA); decl(defaults, keys);
            _decl_list(defaults, keys);
        } else if (lookahead == R_PAREN || lookahead == END) {
            ; // _decl_list -> epsilon
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &decl(map<string, int> &defaults, vector<string> &keys) {
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
    }

    int default_spec(map<string, int> &defaults) {
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

    Parser &column_list(vector<string> &names) {
        if (lookahead == ID) {
            // column_list ->  id _column_list
            names.push_back(id());
            _column_list(names);
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &_column_list(vector<string> &names) {
        if (lookahead == COMMA) {
            // _column_list ->  COMMA id _column_list
            match(COMMA);
            names.push_back(id());
            _column_list(names);
        } else if (lookahead == R_PAREN || lookahead == END){
            ;  // _column_list ->  epsilon
        } else {
            throw ParseError("Syntax error");
        }
    }

    const Insert &insert_stmt() {
        if (lookahead == INSERT) {
            // insert_stmt -> INSERT INTO id L_PAREN column_list R_PAREN
            //                VALUES L_PAREN value_list R_PAREN SEMICOLON
            match(INSERT); match(INTO);
            string table_id = id();
            match(L_PAREN);
            vector<string> columns;
            column_list(columns);
            match(R_PAREN);
            vector<int> values;
            value_list(values);
            match(R_PAREN);
            match(SEMICOLON);
            return Insert(table_id, columns, values);
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &value_list(vector<int> values) {
        if (lookahead == NUM) {
            // value_list ->  num _value_list
            values.push_back(num());
            _value_list(values);
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &_value_list(vector<int> values) {
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
    }

    const Delete &delete_stmt() {
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

    Expr where_clause(Expr &where) {
        if (lookahead == WHERE) {
            // where_clause -> WHERE conjunct_list
            match(WHERE);
            return conjunct_list();
        } else if (lookahead == SEMICOLON || lookahead == END) {
            ;  // where_clause -> epsilon
        } else {
            throw ParseError("Syntax error");
        }

    }

    Expr conjunct_list() {
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

    Expr _conjunct_list() {
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

    Expr boolean() {
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

    Expr operand() {
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

    Type rop() {
        if (lookahead == NEQ || lookahead == EQ
            || lookahead == LT || lookahead == GT
            || lookahead == LEQ || lookahead == GEQ) {
            Type result = lookahead.getType()
            match(result);
            return result;
        } else {
            throw ParseError("Syntax error");
        }
    }

    const Query &query_stmt() {
        if (lookahead == SELECT) {
            // query_stmt -> SELECT select_list FROM id where_clause SEMICOLON
            vector<string> columns;
            match(SELECT); select_list(columns); match(FROM);
            string table_id = id();
            Expr where = where_clause();
            match(SEMICOLON);
            return Query(table_id, columns, where);
        } else {
            throw ParseError("Syntax error");
        }
    }

    Parser &select_list(vector<string> &columns) {
        if (lookahead == MUL) {
            // select_list -> MUL
            columns.push_back("*");
            match(MUL);
        } else {
            // select_list -> column_list
            column_list(columns);
        } else {
            throw ParseError("Syntax error");
        }
    }

    string id() {
        if (lookahead == ID) {
            string result = lookahead.getId();
            lookahead = lexer.next();
            return result;
        } else {
            throw ParseError("Syntax error");
        }
    }

    int &num() {
        if (lookahead == NUM) {
            int result = lookahead.getNumber();
            lookahead = lexer.next();
            return result;
        } else {
            throw ParseError("Syntax error");
        }
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


class Expr {
public:
    Expr(Type t=NONE) : type(t), left(NULL), right(NULL) {}
    int eval(vector<int> record, vector<string> indexes) const;
    bool isNull() const {
        return type == NONE;
    }

    void setLeft(const Expr &other) const {
        if (left != NULL) {
            delete left;
        }

        left = new Expr(other.type);
        *left = other;
    }

    void setRight(const Expr &other) const {
        if (right != NULL) {
            delete right;
        }

        right = new Expr(other.type)
        *right = other;
    }

    void setValue(Token v) {
        value = v;
    }

    void setType(Type t) {
        type = t;
    }

    Expr &operator=(const Expr &rhs) {
        if (rhs.left != NULL) {
            setLeft(*(rhs.left));
        } else {
            left = NULL;
        }

        if (rhs.right != NULL) {
            setRight((*rhs.right));
        } else {
            right = NULL;
        }

        value = rhs.value;
        type = rhs.type;
    }

    ~Expr() {
        if (left != NULL) {
            delete left;
        }

        if (right != NULL) {
            delete right;
        }
    }

    Expr *left;
    Expr *right;
    Token value;
    Type type;
    static Expr NULL_EXPR;
};

Expr Expr::NULL_EXPR;
