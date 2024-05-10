#include "value.h"
#include "context.h"
#include "random.h"

namespace Generator
{
    vector<Value *(*)()> value_classes;
    void initValueClasses() {
        value_classes.push_back(HANDLE_VALUE(NumberValue));
        //value_classes.push_back(HANDLE_VALUE(ObjectValue));
    }
}

Generator::Value::operator string()
{
    return value;
}

void Generator::NumberValue::generate(JSContext *context)
{
    value = context->random->signed_number();
}

void Generator::ObjectValue::generate(JSContext *context)
{
    value = "{}";
}
