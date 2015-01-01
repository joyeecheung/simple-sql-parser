#include "Table.h"
#include "Statements.h"

#ifndef ENGINE_H
#define ENGINE_H

#define MAX_COL 100
#define MAX_KEY 100

namespace ssql {

class Engine {
public:
    Engine() {}
    bool create(const Create &create_stmt);
    // insert the values
    bool insert(const Insert &insert_stmt);
    // delete the records
    int del(const Delete &delete_stmt);
    // query the records
    int query(const Query &query_stmt, vector<vector<int> > &results) const;
    const vector<string> &getColumns(string table_id) const;
    ~Engine() {}
private:
    map<string, Table> tables;
};

}

#endif