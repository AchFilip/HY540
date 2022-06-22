#pragma once
#include "./EvalLanguageIface.h"
#include "./Intepreter.h"

class Sin_Eval : public eval_LanguageIface
{
public:
    Sin_Eval(){};
    Sin_Eval(eval_LanguageIface& s){};

    Interpreter* interpreter;
    Object &PopScopeSpace(void) const
    {
        assert(interpreter);
        Object &top = interpreter->TopScopeSpace();
        interpreter->PopScopeSpace();
        return top;
    }
    void PushScopeSpace(Object &scope) const
    {
        assert(interpreter);
        // interpreter->PushScopeSpace(&scope);
        interpreter->PushScopeSpaceTemp(&scope);
    }
    Object *Parse(const std::string &text) const
    {
        Parser parser;
        return parser.Parse(text);
    }
    const Value Eval(const Object &ast) const
    {
        assert(interpreter);
        return interpreter->Eval(const_cast<Object &>(ast));
    }
    Object &GetEvalParent(void) const
    {
        return *(new Object());
    }
    const std::string ValidateCode(const Object &ast) const
    {
        return "error";
    }
    void Error(const std::string &text) const
    {
    }
    void SetInterpreter(Interpreter* _interpreter)
    {
        this->interpreter = _interpreter;
        assert(interpreter);
    }

    ~Sin_Eval() {}
};

