#include "Table.h"

namespace ssql {

Table::Table(const string table_id, const map<string, int> defs,
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

Table &Table::insert(const vector<string> cols,
                     const vector<int> values) {

    // check no primary key constraint violation
    set<string> colset(cols.begin(), cols.end());
    for (const auto &key : keys) {
        if (colset.find(key) == colset.end()) {
            throw DataBaseError(string("Key ") + key + string(" not found"));
        }
    }

    // all columns should be in the schema of the table
    for(auto col : colset) {
        if (std::find(columns.begin(), columns.end(), col) == columns.end()) {
            throw DataBaseError(string("Column ") + col
                                + string(" is not in the schema"));
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
    for (const auto &record : data) {
        if (conflict(record, new_record)) {
            throw DataBaseError(string("Record already exists"));
        }
    }

    data.push_back(new_record);
    return *this;
}

int Table::del(const Expr expr) {
    if (data.size() <= 0) {
        return 0;
    }

    auto it = data.begin();
    int count = 0;

    // columns occurring in the where clause (if any)
    // should be in the schema of the table
    while (it != data.end()) {
        if (expr.eval(*it, indexes)) {
            it = data.erase(it);
            count++;
        } else {
            it++;
        }
    }

    return count;
}

int Table::query(const vector<string> &names, const Expr expr,
             vector<vector<int> > &results) const {
    int count = 0;
    if (data.size() <= 0) {
        return 0;
    }

    // get queried columns
    if (std::find(names.begin(), names.end(), "*") != names.end()) { // all
        for (const auto &record : data) {
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
        for(const auto &name : names) {
            if (std::find(columns.begin(), columns.end(), name)
                == columns.end()) {
                throw DataBaseError(string("Column ") + name
                                    + string(" is not in the schema"));
            }
        }

        // use int index for reordering
        vector<int> query_indexes;
        for (const auto &name : names) {
            auto it = indexes.find(name);
            query_indexes.push_back(it->second);
        }

        for (const auto &record : data) {
            if (expr.eval(record, indexes)) {
                // columns occurring in the where clause (if any)
                // should be in the schema of the table
                vector<int> reordered;
                for (const auto &idx : query_indexes) {
                    reordered.push_back(record[idx]);
                }
                results.push_back(reordered);
                count++;
            }
        }

        return count;
    }

}

bool Table::conflict(const vector<int> old_record,
              const vector<int> new_record) const {
    for (const auto &key: keys) {
        auto it = indexes.find(key);
        int idx = it->second;
        if (old_record[idx] != new_record[idx]) {
            return false;
        }
    }
    return true;
}

}