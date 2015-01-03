#include <set>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include "Expr.h"

#ifndef TABLE_H
#define TABLE_H

namespace ssql {

using std::set;
using std::map;
using std::string;
using std::vector;
using std::to_string;

class DataBaseError : std::exception {
public:
    DataBaseError(string _msg) : msg(_msg) {}
    ~DataBaseError() throw () {} // Updated
    const char *what() const throw() {
        return msg.c_str();
    }
private:
    string msg;
};

/**
 * Unit of schema.
 */
struct Scheme {
    string name;
    int def;
    bool is_key;
    Scheme(string _name, int _def, bool _is_key)
        : name(_name), def(_def), is_key(_is_key) {}
};

/**
 * Responsible for transactions.
 * Perform semantics analysis that needs the knowledge of its schema.
 */
class Table {
public:
    Table() {}
    // create table with id, default mappins, primary keys
    Table(const string table_id, const map<string, int> defs,
          const vector<string> primary);
    // insert values
    Table &insert(const vector<string> cols, const vector<int> values);
    // delete rows satisfying given expressions
    int del(const Expr expr);
    // query rows satisfying given expressions
    int query(const vector<string> &names, const Expr expr,
              vector<vector<int> > &results) const;
    // get column names of this table
    const vector<string> &getColumns() const {
        return columns;
    }
    // check key conflicts for two records
    bool conflict(const vector<int> old_record,
                  const vector<int> new_record) const;

    ~Table() {}
private:
    string id;

    set<string> keys;  // primary keys
    map<string, int> indexes;  // string names to indexes

    // with same order
    vector<Scheme> schema;
    vector<string> columns;  // column names
    vector<vector<int> > data;
};

}

#endif