#include "context.h"
#include "expr.h"

#include <algorithm>

Generator::JSContext::JSContext() {
    need_update_superset_line_cache = true;
    line = 0;
    variable_count = 0;
    random = new Random();
    random->init();
}

Generator::JSContext::JSContext(Random *random) {
    need_update_superset_line_cache = true;
    line = 0;
    variable_count = 0;
    this->random = random;
    this->random->init();
}

set<string> Generator::JSContext::superset_at_line()
{
    set<string> ans;
    std::for_each(superset.begin(), superset.end(),
                  [&ans, this](map<string, int>::reference i)
                  {
                      if (i.second < this->line)
                      {
                          ans.insert(i.first);
                      }
                  });
    return ans;
}

set<string> &Generator::JSContext::superset_at_line_che()
{
    if (need_update_superset_line_cache)
    {
        superset_at_line_cache = superset_at_line();
        need_update_superset_line_cache = false;
    }
    return superset_at_line_cache;
}

bool Generator::JSContext::contains(const string &name)
{
    return (superset.find(name) != superset.end() && superset[name] < line);
}

void Generator::JSContext::shift_object_location(int begin)
{
    std::for_each(locations.begin(), locations.end(),
                  [&begin, this](map<string, int>::reference i)
                  {
                      if (i.second >= begin)
                      {
                          this->locations[i.first] += 1;
                      }
                  });
    std::for_each(superset.begin(), superset.end(),
                  [&begin, this](map<string, int>::reference i)
                  {
                      if (i.second >= begin)
                      {
                          this->superset[i.first] += 1;
                          need_update_superset_line_cache = true;
                      }
                  });
}

void Generator::JSContext::add_object(Expr *expr, bool verify_unique)
{
    object_pool.push_back(expr);
    ExprWithHandle* exprh = dynamic_cast<ExprWithHandle*>(expr);
    if (exprh != nullptr)
    {
        this->superset[exprh->handle] = this->line;
        superset_at_line_cache.insert(exprh->handle);
        this->line++;
    }
    // update location
    //locations[o->id] = line;
}
Generator::JSContext::~JSContext()
{
    if (random)
    {
        delete random;
    }
}