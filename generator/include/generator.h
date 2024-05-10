#ifndef GENERATOR_H
#define GENERATOR_H

#include "value.h"
#include "context.h"
#include "random.h"
#include "function.h"

namespace Generator
{
    void init();
    class Engine
    {
    private:
        Function *func;

    public:
        Engine();
        ~Engine();
        void generate();
        void generate(const void *data,int len);
        operator string();
    };
}

#endif