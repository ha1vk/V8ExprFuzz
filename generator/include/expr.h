#ifndef EXPR_H
#define EXPR_H

#include "context.h"
#include "value.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace Generator
{
    class JSContext;
    class Value;
    class Expr
    {
    protected:
        string op;

    public:
        virtual ~Expr() {};
        virtual void generate(JSContext *context) = 0;
        virtual operator string() = 0;
    };

    class UnaryExpr : public Expr
    {
        string opr1;

    public:
        virtual void generate(JSContext *context);
        virtual operator string();
    };

    class ExprWithHandle : public Expr
    {
    public:
        string handle;
        bool var_or_let;
        string opr1;
    public:
        virtual ~ExprWithHandle() {}
        virtual void generate(JSContext *context) = 0;
        virtual operator string() = 0;
    };

    class UnaryExprWithHandle : public ExprWithHandle
    {
    public:
        virtual void generate(JSContext *context);
        virtual operator string();
    };

    class BinaryExpr : public Expr
    {
        string opr1;
        string opr2;
        Value *value;

    public:
        BinaryExpr();
        ~BinaryExpr();
        void generate(JSContext *context);
        virtual operator string();
    };

    class BinaryExprWithHandle : public ExprWithHandle
    {
        string opr2;
        Value *value;

    public:
        BinaryExprWithHandle();
        ~BinaryExprWithHandle();
        void generate(JSContext *context);
        virtual operator string();
    };

    extern vector<Expr *(*)()> expr_classes;
    extern vector<Expr *(*)()> non_handle_expr_classes;
    void initExprClasses();

}

#endif