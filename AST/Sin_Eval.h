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
        Object &top = interpreter->TopScopeSpace();
        interpreter->PopScopeSpace();
        return top;
    }
    void PushScopeSpace(Object &scope) const
    {
        interpreter->PushScopeSpace(&scope);
    }
    Object *Parse(const std::string &text) const
    {
        Parser parser;
        return parser.Parse(text);
    }
    const Value Eval(const Object &ast) const
    {
        interpreter->Eval(const_cast<Object &>(ast));
    }
    Object &GetEvalParent(void) const
    {
        
    }
    const std::string ValidateCode(const Object &ast) const
    {

    }
    void Error(const std::string &text) const
    {
    }
    void SetInterpreter(Interpreter* _interpreter)
    {
        interpreter = _interpreter;
    }

    ~Sin_Eval() {}
};

