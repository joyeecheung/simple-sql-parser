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

struct Scheme {
    string name;
    int def;
    bool is_key;
    Scheme(string _name, int _def, bool _is_key)
        : name(_name), def(_def), is_key(_is_key) {}
};

class Table {
public:
    Table() {}
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

    Table &insert(const vector<string> cols, const vector<int> values);
    int del(const Expr expr);
    int query(const vector<string> &names, const Expr expr,
              vector<vector<int> > &results) const;

    const vector<string> &getColumns() const {
        return columns;
    }

    bool conflict(const vector<int> old_record,
                  const vector<int> new_record) const;

    ~Table() {}
private:
    string id;

    set<string> keys;
    map<string, int> indexes;

    // same order
    vector<Scheme> schema;
    vector<string> columns;
    vector<vector<int> > data;
};

}

#endif