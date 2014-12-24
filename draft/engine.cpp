class ParseError : std::exception {
  ParseError(string _msg) : msg(_msg) {}
  ~ParseError() throw () {} // Updated
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

class Engine {
 public:
  Engine &create(const string table_id, const map<string, int> defs,
                 const vector<string> strkeys) {
    if (tables.find(table_id) != tables.end()) {
      throw ParseError(string("Table") + table_id + string("already exists"));
    }
    Table new_table = Table(table_id, defs, strkeys);
    tables[table_id] = new_table;
    return *this;
  }

  // insert the values
  Engine &insert(const string table_id, const vector<string> columns,
                 const vector<int> values) {
    auto it = tables.find(table_id);
    if (it == tables.end()) {
      throw ParseError(string("Cannot find table") + table_id);
    }

    it->insert(columns, values);
    return *this;
  }
  // delete the records
  Engine &del(const string table_id, const Expr expr) {
    auto it = tables.find(table_id);
    if (it == tables.end()) {
      throw ParseError(string("Cannot find table") + table_id);
    }

    it->del(expr);
    return *this;
  }

  // query the records
  Engine &query(const string table_id, const Expr expr,
                vector<int> &results) const {
    auto it = tables.find(table_id);
    if (it == tables.end()) {
      throw ParseError(string("Cannot find table") + table_id);
    }

    it->query(expr, results);
    return *this;
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
    }
  }

  Table &insert(const vector<string> cols, const vector<int> values) {
    if (columns.size() < values.size()) {
      throw ParseError(string("Too many values");
    } else if (columns.size() == values.size()) {
      // check no missed key
      for (auto strkey : strkeys) {
        if (std::find(cols.begin(), cols.end(), strkey) == cols.end()) {
          throw ParseError(string("Key ") + strkey + string(" not found"));
        }
      }

      // check not already exists
      for (auto record : data) {
        if (conflict(record, cols, values)) {
          throw ParseError(string("Record already exists"));
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
      if (match(*it, expr)) {
        data.erase(it++);
      } else {
        it++;
      }
    }
  }

  Table &query(const Expr expr, vector<vector<int> > &results) const {
    auto it = data.begin()
    while (it != data.end()) {
      if (match(*it, expr)) {
        results.push_back(*it);
        it++;
      } else {
        it++;
      }
    }
  }

  vector<string> getKeys() const {
    return strkeys;
  }

  bool match(const vector<int> record, const Expr expr) const {
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

  vector<vector<int>> data;
  map<string, int> indexes;
  vector<Column> columns;
};
