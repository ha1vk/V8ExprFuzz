#include "generator.h"

Generator::Engine::Engine() {
    func = nullptr;
}

Generator::Engine::~Engine() {
    delete func;
}

void Generator::init()
{
    initValueClasses();
    initExprClasses();
}

void Generator::Engine::generate() {
    if (func) delete func;
    func = new Function();
    func->generate_exprs();
}
void Generator::Engine::generate(const void *data,int len) {
    if (func) delete func;
    func = new Function(data,len);
    func->generate_exprs();
}
Generator::Engine::operator string() {
    string s = func->operator string();
    s.append("let r1=foo(1,0);\n");
    s.append("%PrepareFunctionForOptimization(foo);\n");
    s.append("let r2=foo(1,1);\n");
    s.append("%OptimizeFunctionOnNextCall(foo);\n");
    s.append("let r3 = foo(1,0);\n");
    s.append("let r4 = foo(1,1);\n");
    s.append("for (var i=0;i<r1.length;i++) {\n");
    s.append("if (isNaN(r1[i]) && isNaN(r3[i])) continue;\n");
    s.append("if (r1[i] !== r3[i]  && (typeof(r1[i]) == 'number' || typeof(r3[i]) == 'number') ) %SystemBreak();\n");
    s.append("}\n");
    s.append("for (var i=0;i<r1.length;i++) {\n");
    s.append("if (isNaN(r2[i]) && isNaN(r4[i])) continue;\n");
    s.append("if (r2[i] !== r4[i]  && (typeof(r2[i]) == 'number' || typeof(r4[i]) == 'number') ) %SystemBreak();\n");
    s.append("}\n");
    return s;
}

extern "C" {
    #include <string.h>
    void *Engine() {
        Generator::init();
        return new Generator::Engine();
    }
    void generator(void *e,char *in,int in_len,char **out,int *out_len) {
        Generator::Engine *engine = (Generator::Engine *)e;
        engine->generate(in,in_len);
        string s = engine->operator string();
        *out = (char *)malloc(s.length());
        *out_len = s.length();
        memcpy(*out,s.c_str(),*out_len);
    }
}