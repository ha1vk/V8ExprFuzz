#include "expr.h"
#include "random.h"
#include "value.h"

void Generator::UnaryExpr::generate(JSContext *context)
{
    set<string> objs = context->superset_at_line();
    opr1 = context->random->choice_set(objs, nullptr);
    const char *ops[] = {"x++", "x--", "++x", "--x"};
    op = CHOICE(ops);
}

Generator::UnaryExpr::operator string()
{
    string ans = "";
    if (op == "x++")
    {
        ans.append(opr1);
        ans.append("++");
    }
    else if (op == "x--")
    {
        ans.append(opr1);
        ans.append("--");
    }
    else if (op == "++x")
    {
        ans.append("++");
        ans.append(opr1);
    }
    else if (op == "--x")
    {
        ans.append("--");
        ans.append(opr1);
    }
    else
    {
        ans.append(op);
        ans.append(opr1);
    }
    ans.append(";");
    return ans;
}

void Generator::UnaryExprWithHandle::generate(JSContext *context)
{
    set<string> objs = context->superset_at_line();
    opr1 = context->random->choice_set(objs, nullptr);
    const char *ops[] = {"!", "x++", "x--", "++x", "--x", "~", "-", "+","abs","acos","acosh","asin","asinh","atan","atan2","atanh","cbrt","ceil","clz32","cos","cosh","exp","expm1","floor","fround","log","log10","log1p","log2","round","sign","sin","sinh","sqrt","tan","tanh","trunc"};
    op = CHOICE(ops);
    char buf[40];
    sprintf(buf, "v%d", context->variable_count);
    handle = buf;
    var_or_let = context->random->gbool();
    context->variable_count++;
    context->add_object(this);
}

Generator::UnaryExprWithHandle::operator string()
{
    string ans = "";
    if (var_or_let)
    {
        ans.append("var ");
    }
    else
    {
        ans.append("let ");
    }
    ans.append(handle);
    ans.append(" = ");
    if (op == "x++")
    {
        ans.append(opr1);
        ans.append("++");
    }
    else if (op == "x--")
    {
        ans.append(opr1);
        ans.append("--");
    }
    else if (op == "++x")
    {
        ans.append("++");
        ans.append(opr1);
    }
    else if (op == "--x")
    {
        ans.append("--");
        ans.append(opr1);
    } else if (isalpha(op[0])) {
        ans.append("Math.");
        ans.append(op);
        ans.append("(");
        ans.append(opr1);
        ans.append(")");
    }
    else
    {
        ans.append(op);
        ans.append(opr1);
    }
    ans.append(";");
    return ans;
}

Generator::BinaryExpr::BinaryExpr()
{
    value = nullptr;
}

Generator::BinaryExpr::~BinaryExpr()
{
    delete value;
}

void Generator::BinaryExpr::generate(JSContext *context)
{
    set<string> objs = context->superset_at_line();
    opr1 = context->random->choice_set(objs, nullptr);
    if (context->random->gbool())
        opr2 = context->random->choice_set(objs, nullptr);
    else
    {
        if (value)
            delete value;
        value = CHOICE_OBJ(value_classes)();
        value->generate(context);
        opr2 = value->operator string();
    }
    const char *ops[] = {"+=", "-=", "*=", "/=", "|=", "&=", "||=", "&&=", "^=", "%=", ">>=", ">>>=", "<<="};
    op = CHOICE(ops);
}

Generator::BinaryExpr::operator string()
{
    string ans = opr1;
    ans.append(op);
    ans.append(opr2);
    ans.append(";");
    return ans;
}

Generator::BinaryExprWithHandle::BinaryExprWithHandle()
{
    value = nullptr;
}

Generator::BinaryExprWithHandle::~BinaryExprWithHandle()
{
    delete value;
}

void Generator::BinaryExprWithHandle::generate(JSContext *context)
{
    set<string> objs = context->superset_at_line();
    opr1 = context->random->choice_set(objs, nullptr);
    if (context->random->gbool())
        opr2 = context->random->choice_set(objs, nullptr);
    else
    {
        if (value)
            delete value;
        value = CHOICE_OBJ(value_classes)();
        value->generate(context);
        opr2 = value->operator string();
    }
    const char *ops[] = {"+", "-", "*", "/", "|", "&", "||", "&&", "^", "%", ">", "<", "==", "!=", "!==", ">>", ">>>", "<<", "max", "min","hypot","imul","pow","is"};
    op = CHOICE(ops);
    char buf[40];
    sprintf(buf, "v%d", context->variable_count);
    handle = buf;
    var_or_let = context->random->gbool();
    context->variable_count++;
    context->add_object(this);
}

Generator::BinaryExprWithHandle::operator string()
{
    string ans = "";
    if (var_or_let)
    {
        ans.append("var ");
    }
    else
    {
        ans.append("let ");
    }
    ans.append(handle);
    ans.append(" = ");
    if (op == "max" || op == "min" || op == "imul" || op == "hypot" || op == "pow")
    {
        ans.append("Math.");
        ans.append(op);
        ans.append("(");
        ans.append(opr1);
        ans.append(",");
        ans.append(opr2);
        ans.append(")");
    } else if (op == "is") {
        ans.append("Object.");
        ans.append(op);
        ans.append("(");
        ans.append(opr1);
        ans.append(",");
        ans.append(opr2);
        ans.append(")");
    } else
    {
        ans.append(opr1);
        ans.append(" ");
        ans.append(op);
        ans.append(" ");
        ans.append(opr2);
    }
    ans.append(";");
    return ans;
}

namespace Generator
{
    #define HANDLE_EXPR(clazz) (HANDLE(Expr,clazz))
    vector<Expr *(*)()> expr_classes;
    vector<Expr *(*)()> non_handle_expr_classes;

    void initExprClasses() {
        expr_classes.push_back(HANDLE_EXPR(UnaryExpr));
        expr_classes.push_back(HANDLE_EXPR(UnaryExprWithHandle));
        expr_classes.push_back(HANDLE_EXPR(BinaryExpr));
        expr_classes.push_back(HANDLE_EXPR(BinaryExprWithHandle));

        non_handle_expr_classes.push_back(HANDLE_EXPR(UnaryExpr));
        non_handle_expr_classes.push_back(HANDLE_EXPR(BinaryExpr));
    }
}
