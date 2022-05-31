#include "./Object.h"

#define LANG (*langIface)

class eval_LanguageIface{
public:
    virtual Object&             PopScopeSpace(void)                 const = 0;
    virtual void                PushScopeSpace(Object& scope)       const = 0;
    virtual Object*             Parse(const std::string& text)      const = 0;
    virtual const Value         Eval(const Object& ast)             const = 0;
    virtual Object&             GetEvalParent(void)                 const = 0;
    virtual const std::string   ValidateCode(const Object& ast)     const = 0;
    virtual void                Error(const std::string& text)      const = 0;
    static void                 Set(eval_LanguageIface* iface);
    virtual ~eval_LanguageIface(){}
};

static  eval_LanguageIface* langIface = nullptr;

void    eval_LanguageIface::Set(eval_LanguageIface* iface)
{ 
    langIface = iface;
}

