class DataBaseError : std::exception {
    DataBaseError(string _msg) : msg(_msg) {}
    ~DataBaseError() throw () {} // Updated
    const char *what() const throw() {
        return msg.c_str();
    }
private:
    string msg;
};


struct Column {
    string name;
    int def;
    bool is_key;
    Column(string _name, int def, bool _is_key)
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

                vector<vector<int> > results;
                vector<string> names;
                int number = engine.query(stmt, names, results);
                if (number > 0) {
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
    bool create(const Statement &stmt) {
        const Create &create_stmt = dynamic_cast<const Create &>(stmt);
        if (create_stmt == NULL) {
            return false;
        }

        string table_id = create_stmt.getId();
        if (tables.find(table_id) != tables.end()) {
            throw DataBaseError(string("Table") + table_id + string("already exists"));
        }

        Table new_table = Table(table_id, create_stmt.getDefaults(),
                                create_stmt.getKeys());
        tables[create_stmt.id] = new_table;
        return true;
    }

    // insert the values
    bool insert(const Statement &stmt) {
        const Insert &insert_stmt = dynamic_cast<const Insert &>(stmt);
        if (insert_stmt == NULL) {
            return false;
        }

        string table_id = insert_stmt.getId();
        auto it = tables.find(table_id);
        if (it == tables.end()) {
            throw DataBaseError(string("Cannot find table") + table_id);
        }

        it->insert(insert_stmt.getColumns(), insert_stmt.getValues());
        return true;
    }

    // delete the records
    bool del(const Statement &stmt) {
        const Delete &delete_stmt = dynamic_cast<const Delete &>(stmt);
        if (delete_stmt == NULL) {
            return false;
        }

        string table_id = delete_stmt.getId();
        auto it = tables.find(table_id);
        if (it == tables.end()) {
            throw DataBaseError(string("Cannot find table") + table_id);
        }

        it->del(delete_stmt.getWhere());
        return true;
    }

    // query the records
    Engine &query(const Statement &stmt, vector<strings> &names,
                  vector<vector<int> > &results) const {
        const Query &query_stmt = dynamic_cast<const Query &>(stmt);
        if (query_stmt == NULL) {
            return false;
        }

        string table_id = query_stmt.getId();
        auto it = tables.find(table_id);
        if (it == tables.end()) {
            throw DataBaseError(string("Cannot find table") + table_id);
        }

        it->query(names, query_stmt.getWhere(), results);
        return bool;
    }

    vector<string> &getColumnNames(string table_id) {
        auto it = tables.find(table_id);
        if (it == tables.end()) {
            throw DataBaseError(string("Cannot find table") + table_id);
        }

        return it->getColumnNames();
    }

private:
    map<id, Table> tables;
};

class Table {
public:
    Table(const string table_id, const map<string, int> defs,
          const vector<string> _strkeys)
        : id(table_id), strkeys(_strkeys) {
        int counter = 0;
        for (auto it = defs.begin(); it != defs.end(); ++it) {
            Column new_column = Column(it->first, it->second; counter, false);
            indexes[it->first] = counter;
            columns.push_back(new_column);
            columnNames.push_back(it->first);
        }
    }

    Table &insert(const vector<string> cols, const vector<int> values) {
        if (columns.size() < values.size()) {
            throw DataBaseError(string("Too many values");
        } else if (columns.size() == values.size()) {
            // check no missed key
            for (auto strkey : strkeys) {
                if (std::find(cols.begin(), cols.end(), strkey) == cols.end()) {
                    throw DataBaseError(string("Key ") + strkey + string(" not found"));
                }
            }

            // check not already exists
            for (auto record : data) {
                if (conflict(record, cols, values)) {
                    throw DataBaseError(string("Record already exists"));
                }
            }

            // fill in defaults
            vector<int> new_record(columns.size());
            // fill in default data
            for (int i = 0; i < columns.size(); ++i) {
                new_record[i] = columns[i].def;
            }

            // fill in available data
            for (int i = 0; i < cols.size(); ++i) {
                int index = indexes[col[i]];
                new_record[index] = values[i];
            }

        }
    }

    Table &del(const Expr expr) {
        // match the expression against each record
        auto it = data.begin()
        while (it != data.end()) {
            if (expr.eval(*it, indexes)) {
                data.erase(it++);
            } else {
                it++;
            }
        }
    }

    Table &query(vector<string> &names, const Expr expr,
                 vector<vector<int> > &results) const {
        vector<string> query_columns = query_stmt.getColumns();
        if (std::find(query_columns.begin(), query_columns.end(), '*')) {
            std::copy(columnNames.begin(), columnNames.end(), names.begin());
        } else {
            std::copy(query_columns.begin(), query_columns.end(), names.begin());
        }

        vector<int> query_indexes;
        for (auto name : names) {
            query_indexes.push_back(indexes[name]);
        }

        auto it = data.begin()
        while (it != data.end()) {
            const vector<int> &record = *it;
            if (expr.eval(record, indexes)) {
                vector<int> reordered;
                for (index : query_indexes) {
                    reordered.push_back(record[index]);
                }

                results.push_back(reordered);
                it++;
            } else {
                it++;
            }
        }

    }

    vector<string> getKeys() const {
        return strkeys;
    }

    const vector<string> &getColumnNames() const {
        return columnNames;
    }

    bool conflict(const vector<int> record, const vector<string> cols,
                  const vector<int> values) const {
        for (int i = 0; i < cols.size(); ++i) {
            int index = indexes[cols[i]];
            if (columns[index].is_key) {
                int value = values[i];
                if (record[index] != value) {
                    return false;
                }
            }
        }
        return true;
    }

private:
    string id;

    vector<string> strkeys;

    vector<vector<int> > data;
    map<string, int> indexes;
    vector<Column> columns;
    vector<string> columnNames;
};
