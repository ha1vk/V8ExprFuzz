#ifndef CONTEXT_H
#define CONTEXT_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include "random.h"
#include "expr.h"

using std::map;
using std::set;
using std::string;
using std::vector;

namespace Generator
{
    class Expr;
    class JSContext
    {
    private:
        // map<string, int> superset;
        map<string, int> locations;
        vector<Expr *> object_pool;
        set<string> superset_at_line_cache;
        bool need_update_superset_line_cache;

    public:
        map<string, int> superset;
        int line;
        int variable_count;
        Random *random;
        JSContext();
        JSContext(Random *random);
        ~JSContext();
        set<string> superset_at_line();
        set<string> &superset_at_line_che();
        void add_object(Expr *expr, bool verify_unique = false);
        bool contains(const string &name);
        void shift_object_location(int begin);
    };
}

#endif