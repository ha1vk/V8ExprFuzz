#ifndef JS_Value_H
#define JS_Value_H

#include "context.h"
#include <vector>
#include <map>
#include <string>

using std::map;
using std::vector;
using std::string;

#define HANDLE(parent_class,clazz) ((parent_class * (*)(void)) instanceObj<clazz>)
#define HANDLE_VALUE(clazz) (HANDLE(Value,clazz))

template <class T>
const T *instanceObj()
{
    T *obj = new T();
    return obj;
}

namespace Generator
{
    class JSContext;
    class Value
    {
    public:
        string value;
        virtual void generate(JSContext *context) = 0;
        // virtual bool mutate(JSContext *context);
        // virtual void merge_fix(map<Object *, Object *> &merge_map);
        virtual operator string();
    };

    class NumberValue : public Value
    {
    public:
        virtual void generate(JSContext *context);
    };
    class ObjectValue : public Value
    {
    public:
        virtual void generate(JSContext *context);
    };
    extern vector<Value *(*)()> value_classes;
    void initValueClasses();
}
#endif
