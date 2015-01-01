#include "Engine.h"

namespace ssql {

bool Engine::create(const Create &create_stmt) {
    // check the table is not created before
    string table_id = create_stmt.getId();
    if (tables.find(table_id) != tables.end()) {
        throw DataBaseError(string("Table ") + table_id
                            + string(" already exists"));
    }

    // check no multiple primary keys
    const vector<vector<string> > &keys = create_stmt.getKeys();
    if (keys.size() > 1) {
        throw DataBaseError("Multiple primary key definitions");
    }

    // check no duplicate column definitions
    const multimap<string, int> &defs = create_stmt.getDefaults();
    map<string, int> unique_defs;
    for(auto it = defs.begin(); it != defs.end();
             it = defs.upper_bound(it->first)) {
        if (defs.count(it->first) > 1) {
           throw DataBaseError(string("Multiple definitions for ")
                               + it->first);
        } else {
            unique_defs[it->first] = it->second;
        }
    }

    // check all primary keys have definitions
    vector<string> primary;
    if (keys.size() > 0) {
        primary = keys[0];
        for (const auto &key: primary) {
            if (defs.find(key) == defs.end()) {
                throw DataBaseError(string("Undefined key ") + key);
            }
        }
    } else {  // no keys
        for (const auto &kv  : defs) {
            primary.push_back(kv.first);
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
    tables[table_id] = new_table;
    return true;
}

// insert the values
bool Engine::insert(const Insert &insert_stmt) {
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
    for (const auto &col : columns) {
        if (checked.find(col) != checked.end()) {
            throw DataBaseError(string("Duplicate column ") + col);
        } else {
            checked.insert(col);
        }
    }

    // no primary key constraint violation(all have/already in)
    // all columns should be in the schema of the table
    it->second.insert(columns, values);
    return true;
}

// delete the records
int Engine::del(const Delete &delete_stmt) {
    // the table should exist
    string table_id = delete_stmt.getId();
    auto it = tables.find(table_id);
    if (it == tables.end()) {
        throw DataBaseError(string("Cannot find table ") + table_id);
    }

    // columns occurring in the where clause (if any)
    // should be in the schema of the table
    return it->second.del(delete_stmt.getWhere());
}

// query the records
int Engine::query(const Query &query_stmt,
                  vector<vector<int> > &results) const {
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
    return it->second.query(query_stmt.getColumns(), query_stmt.getWhere(), results);
}

const vector<string> &Engine::getColumns(string table_id) const {
    auto it = tables.find(table_id);
    if (it == tables.end()) {
        throw DataBaseError(string("Cannot find table ") + table_id);
    }

    return it->second.getColumns();
}

}