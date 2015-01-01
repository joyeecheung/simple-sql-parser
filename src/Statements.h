#include "Expr.h"

#include <set>
using std::multimap;
using std::multiset;

#ifndef STATEMENTS_H
#define STATEMENTS_H

namespace ssql{

class Statement {
public:
    Statement(string _id) : id(_id) {}
    string getId() const { return id; }
private:
    string id;
};

/**
 * Create statement. Consists of id, defaults and keys.
 */
class Create : public Statement {
public:
    Create(const string _id, const multimap<string, int> &_default_spec,
           const vector<vector<string> > &_keys) :
            Statement(_id),
            default_spec(_default_spec),
            keys(_keys) {}

    const multimap<string, int> &getDefaults() const {
        return default_spec;
    }

    const vector<vector<string> > &getKeys() const {
        return keys;
    }

private:
    multimap<string, int> default_spec;
    vector<vector<string> > keys;
};

/**
 * Insert statement. Consists of id, columns and values(paired with index).
 */
class Insert : public Statement {
public:
    Insert(const string _id, const vector<string> &_columns,
           const vector<int> &_values) :
            Statement(_id),
            columns(_columns),
            values(_values) {}

    const vector<string> &getColumns() const {
        return columns;
    }

    const vector<int> &getValues() const {
        return values;
    }

private:
    vector<string> columns;
    vector<int> values;
};

/**
 * Delete statement. Consists of id and a where expression.
 */
class Delete : public Statement {
public:
    Delete(const string _id, const Expr &_where)
        : Statement(_id), where(_where) {}
    const Expr &getWhere() const {
        return where;
    }
private:
    Expr where;
};

/**
 * Query statement. Consists of id, columns, and a where expression.
 */
class Query : public Statement {
public:
    Query(const string _id, const vector<string> _columns,
          const Expr &_where)
        : Statement(_id), columns(_columns), where(_where) {}
    const vector<string> &getColumns() const {
        return columns;
    }
    const Expr &getWhere() const {
        return where;
    }
private:
    vector<string> columns;  // can contain '*'
    Expr where;
};

}
#endif