class DataBaseError : std::exception {
    DataBaseError(string _msg) : msg(_msg) {}
    ~DataBaseError() throw () {} // Updated
    const char *what() const throw() {
        return msg.c_str();
    }
private:
    string msg;
};

#define MAX_COL 100
#define MAX_KEY 100

struct Scheme {
    string name;
    int def;
    bool is_key;
    Scheme(string _name, int def, bool _is_key)
        : name(_name), def(_def), is_key(_is_key) {}
};


int main(int argc, char const *argv[]) {
    Lexer *lexptr;
    ofstream out;
    ifstream in;  // must be in the scope until stop scanning

    if (argc > 1) {
        in.open(argv[1], ifstream::in);
        if (in.is_open()) {
            lexptr = new Lexer(in);
        } else {
            cout << "Fail to open " << argv[1] << '\n';
            exit(1);
        }
    } else {
        lexptr = new Lexer(cin);
    }

    Parser parser(*lexptr);
    Engine engine;

    while (!parser.isEnd()) {
        try {
            Type next = parser.next_stmt_type();
            if (next == CREATE) {
                Create create_stmt = parser.create_stmt();
                engine.create(create_stmt);
                cout << "Created table " << create_stmt.getId() << "\n";
            } else if (next == INSERT) {
                Insert insert_stmt = parser.insert_stmt();
                int number = engine.insert(insert_stmt);
                cout << "Inserted " << number << " rows into table ";
                cout << insert_stmt.getId() << "\n";
            } else if (next == DELETE) {
                Delete delete_stmt = parser.delete_stmt();
                int number = engine.del(insert_stmt);
                cout << "Deleted " << number << " rows from table ";
                cout << delete_stmt.getId() << "\n";
            } else if (next == SELECT) {
                Query query_stmt = parser.query_stmt();
                string table_id = query_stmt.getId();

                vector<vector<int> > results;
                vector<string> names = query_stmt.getColumns();

                int number = engine.query(query_stmt, results);
                if (number > 0) {
                    if (std::find(name.begin(), names.end(), "*") != names.end()) {
                        names = engine.getColumns(table_id);
                    }

                    for (auto name : names) {
                        cout << name << '\t';
                    }
                    cout << '\n';

                    for (auto record : results) {
                        for (auto col : record) {
                            cout << col << '\t'
                        }
                        cout << '\n';
                    }
                    cout << number <<< " matching rows in " << query_stmt.getId() << "\n";
                } else {
                    cout << "No matching rows in " << query_stmt.getId() << "\n";
                }
            }
        } catch (LexError e) {
            cout << lexptr->getLine() << ": " << e.what()  << '\n';
        } catch (ParseError e) {
            cout << lexptr->getLine() << ": " << e.what() << '\n';
        } catch(DataBaseError e) {
            cout << lexptr->getLine() << ": " << e.what()  << '\n';
        }
    }

    return 0;
}


class Engine {
public:
    bool create(const Create &create_stmt) {
        // check the table is not created before
        string table_id = create_stmt.getId();
        if (tables.find(table_id) != tables.end()) {
            throw DataBaseError(string("Table ") + table_id + string(" already exists"));
        }

        // check no multiple primary keys
        const vector<vector<string> > &keys = create.getKeys();
        if (keys.size() > 1) {
            throw DataBaseError("Multiple primary key definitions"));
        }

        // check no duplicate column definitions
        const multimap<string, int> &defs = create.getDefaults();
        map<string, int> unique_defs;
        for(auto it = defs.begin(); it != defs.end(); it = defs.upper_bound(it->first)) {
            if (defaults.count(it->first) > 1) {
               throw DataBaseError(string("Multiple definitions for ") + it->first);
            } else {
                unique_defs[it->first] = it->second;
            }
        }

        // check all primary keys have definitions
        const vector<string> &primary = keys[0];
        for (auto key: primary) {
            if (defs.find(key) == defs.end()) {
                throw DataBaseError(string("Undefined key ") + key);
            }
        }

        if (unique_defs.size() > MAX_COL) {
            throw DataBaseError(string("Number of columns should be no more than ")
                                + to_string(MAX_COL));
        }

        if (primary.size() > MAX_KEY) {
            throw DataBaseError(string("Number of keys should be no more than ")
                                + to_string(MAX_KEY));
        }

        Table new_table = Table(table_id, unique_defs, primary);
        tables[create_stmt.id] = new_table;
        return true;
    }

    // insert the values
    bool insert(const Insert &insert_stmt) {
        // the table should exist
        string table_id = insert_stmt.getId();
        auto it = tables.find(table_id);
        if (it == tables.end()) {
            throw DataBaseError(string("Cannot find table ") + table_id);
        }

        set<string> checked;
        const vector<string> &columns = insert_stmt.getColumns();
        const vector<int> &values = insert_stmt.getValues();

        // # of columns should equal to # of values
        if (columns.size() != values.size()) {
            throw DataBaseError("Numbers of columns and values do not match");
        }

        // no duplicate columns
        for (auto col : columns) {
            if (checked.find(col) != checked.end()) {
                throw DataBaseError(string("Duplicate column ") + col);
            } else {
                checked.insert(col);
            }
        }

        // no primary key constraint violation(all have/already in)
        // all columns should be in the schema of the table
        return it->insert(columns, values);
    }

    // delete the records
    int del(const Delete &delete_stmt) {
        // the table should exist
        string table_id = delete_stmt.getId();
        auto it = tables.find(table_id);
        if (it == tables.end()) {
            throw DataBaseError(string("Cannot find table ") + table_id);
        }

        // columns occurring in the where clause (if any)
        // should be in the schema of the table
        return it->del(delete_stmt.getWhere());
    }

    // query the records
    int query(const Insert &query_stmt, vector<vector<int> > &results) const {
        // the table should exist
        string table_id = query_stmt.getId();
        auto it = tables.find(table_id);
        if (it == tables.end()) {
            throw DataBaseError(string("Cannot find table ") + table_id);
        }

        // all columns (except *) in the select list should be in
        // the schema of the table
        // columns occurring in the where clause (if any)
        // should be in the schema of the table
        return it->query(query_stmt.getColumns(), query_stmt.getWhere(), results);
    }

    const vector<string> &getColumns(string table_id) {
        auto it = tables.find(table_id);
        if (it == tables.end()) {
            throw DataBaseError(string("Cannot find table ") + table_id);
        }

        return it->getColumns();
    }

private:
    map<string, Table> tables;
};

class Table {
public:
    Table(const string table_id, const map<string, int> defs,
          const vector<string> primary)
        : id(table_id), keys(primary.begin(), primary.end()) {
        int counter = 0;
        for (auto it = defs.begin(); it != defs.end(); ++it) {
            Scheme new_scheme = Scheme(it->first, it->second,
                                       keys.find(it->first) != keys.end());
            indexes[it->first] = counter++;
            schema.push_back(new_scheme);
            columns.push_back(it->first);
        }

    }

    Table &insert(const vector<string> cols, const vector<int> values) {

        // check no primary key constraint violation
        set<string> colset(cols.begin(), cols.end());
        for (auto key : keys) {
            if (colset.find(key) == colset.end()) {
                throw DataBaseError(string("Key ") + key + string(" not found"));
            }
        }

        // all columns should be in the schema of the table
        for(col : colset) {
            if (std::find(columns.begin(), columns.end(), col) == columns.end()) {
                throw DataBaseError(string("Column ") + col + string(" is not in the schema"));
            }
        }

        vector<int> new_record(columns.size());
        // fill in default data
        for (int i = 0; i < columns.size(); ++i) {
            new_record[i] = schema[i].def;
        }

        // fill in available data
        for (int i = 0; i < cols.size(); ++i) {
            int index = indexes[cols[i]];
            new_record[index] = values[i];
        }

        // check not already exists
        for (auto record : data) {
            if (conflict(record, new_record)) {
                throw DataBaseError(string("Record already exists"));
            }
        }

        data.push_back(new_record);
    }

    int del(const Expr expr) {
        auto it = data.begin();
        int count = 0;

        // columns occurring in the where clause (if any)
        // should be in the schema of the table
        while (it != data.end()) {
            if (expr.eval(*it, indexes)) {
                data.erase(it++);
                count++;
            } else {
                it++;
            }
        }

        return count;
    }

    int query(const vector<string> &names, const Expr expr,
                 vector<vector<int> > &results) const {
        int count = 0;

        // get queried columns
        if (std::find(names.begin(), names.end(), "*")) { // all
            for (auto record : data) {
                if (expr.eval(record, indexes)) {
                    // columns occurring in the where clause (if any)
                    // should be in the schema of the table
                    results.push_back(record);
                    count++;
                }
            }
            return count;
        } else {  // selected columns
            // all columns (except *) in the select list should be in
            // the schema of the table
            for(name : names) {
                if (std::find(columns.begin(), columns.end(), name) == columns.end()) {
                    throw DataBaseError(string("Column ") + name + string(" is not in the schema"));
                }
            }

            // use int index for reordering
            vector<int> query_indexes;
            for (auto name : names) {
                query_indexes.push_back(indexes[name]);
            }

            for (auto record : data) {
                if (expr.eval(record, indexes)) {
                    // columns occurring in the where clause (if any)
                    // should be in the schema of the table
                    vector<int> reordered;
                    for (idx : query_indexes) {
                        reordered.push_back(record[idx]);
                    }
                    results.push_back(reordered);
                    count++;
                }
            }

            return count;
        }

    }

    const vector<string> &getColumns() const {
        return columns;
    }

    bool conflict(const vector<int> old_record, const vector<int> new_record) const {
        for (key: keys) {
            int idx = indexes[key];
            if (old_record[idx] != new_record[idx]) {
                return false;
            }
        }
        return true;
    }


private:
    string id;

    set<string> keys;
    map<string, int> indexes;

    // same order
    vector<Scheme> schema;
    vector<string> columns;
    vector<vector<int> > data;
};
