#include "function.h"
#include "config.h"
#include "random.h"
#include "expr.h"
#include <algorithm>

Generator::Function::Function()
{
    context = new JSContext();
    // 最开始有两个参数
    context->superset["a"] = -1;
    context->superset["b"] = -1;
}
Generator::Function::Function(const void *data,int len) {
    context = new JSContext(new Random(data,len));
    // 最开始有两个参数
    context->superset["a"] = -1;
    context->superset["b"] = -1;
}
Generator::Function::~Function()
{
    delete context;
    std::for_each(exprs.begin(), exprs.end(),
                  [](Expr *expr)
                  {
                      delete expr;
                  });
}

int Generator::Function::expr_count()
{
    return exprs.size();
}

bool Generator::Function::full()
{
    return expr_count() >= JSConfig::max_expr_count;
}

Generator::Expr *Generator::Function::generate_expr()
{
    Expr *expr;
    if (context->variable_count < JSConfig::max_variable_count)
    {
        expr = CHOICE_OBJ(expr_classes)();
    }
    else
    {
        expr = CHOICE_OBJ(non_handle_expr_classes)();
    }
    expr->generate(context);
    return expr;
}

void Generator::Function::generate_exprs()
{
    int count = context->random->range(2,JSConfig::max_expr_count);
    for (int i = 0; i < count; i++)
        append_expr();
}

bool Generator::Function::append_expr()
{
    if (full())
        return false;
    context->line = exprs.size();
    Expr *expr = generate_expr();
    exprs.push_back(expr);
    return true;
}

Generator::Function::operator string()
{
    string s = "function foo(a,b) {\n";
    std::for_each(exprs.begin(), exprs.end(),
                  [&s](Expr *expr)
                  {
                      s.append(expr->operator string());
                      s.append("\n");
                  });
    s.append("return [");
    char buf[12];
    int count = context->variable_count;
    for (int i = 0; i < count; i++)
    {
        sprintf(buf, "v%d", i);
        s.append(buf);
        s.append(",");
    }
    s.append("a,b]\n");
    s.append("}\n");
    return s;
}