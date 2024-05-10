#ifndef FUNCTION_H
#define FUNCTION_H

#include "context.h"
#include "expr.h"

#include <vector>

using std::vector;

namespace Generator
{
    class Function
    {
    private:
        vector<Expr *> exprs;
        JSContext *context;

    public:
        Function();
        Function(const void *data,int len);
        ~Function();
        int expr_count();
        bool full();
        Expr *generate_expr();
        /*
         * For dumb fuzz
         */
        void generate_exprs();
        bool append_expr();
        // A4
        bool mutate_expr();
        void merge_expr(Expr *expr, int line);
        operator string();
    };
}

#endif