#include "./Object.h"
#include "./Intepreter.h"

#define LANG (*langIface)

class eval_LanguageIface
{
public:
    virtual Object &PopScopeSpace(void) const = 0;
    virtual void PushScopeSpace(Object &scope) const = 0;
    virtual Object *Parse(const std::string &text) const = 0;
    virtual const Value Eval(const Object &ast) const = 0;
    virtual Object &GetEvalParent(void) const = 0;
    virtual const std::string ValidateCode(const Object &ast) const = 0;
    virtual void Error(const std::string &text) const = 0;
    static void Set(eval_LanguageIface *iface);
    virtual ~eval_LanguageIface() {}
};

static eval_LanguageIface *langIface = nullptr;

void eval_LanguageIface::Set(eval_LanguageIface *iface)
{
    langIface = iface;
}

class Sin_Eval : public eval_LanguageIface
{
    Interpreter &interpreter;
    Object &PopScopeSpace(void) const
    {
        Object &top = interpreter.TopScopeSpace();
        interpreter.PopScopeSpace();
        return top;
    }
    void PushScopeSpace(Object &scope) const
    {
        interpreter.PushScopeSpace(&scope);
    }
    Object *Parse(const std::string &text) const
    {
    }
    const Value Eval(const Object &ast) const
    {
        interpreter.Eval(const_cast<Object &>(ast));
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
    void SetInterpreter(Interpreter &_interpreter)
    {
        interpreter = _interpreter;
    }
    ~Sin_Eval() {}
};
