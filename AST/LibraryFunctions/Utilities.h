#pragma once
#include "../TreeTags.h"
#include "../Object.h"
#include "../EvalDispatcher.h"
#include "../DebugAST.h"
#include "../EvalLanguageIface.h"

#define PRINT_BLUE_LINE(to_print) std::cout << "\033[1;36m" << to_print << "\033[0m\n"
#define PRINT_YELLOW_LINE(to_print) std::cout << "\033[1;33m" << to_print << "\033[0m\n"
#define NORMAL_PRINT_LINE(to_print) std::cout << to_print << "\n"

const Value *GetArgument(Object &env, unsigned argNo, const std::string &optArgName)
{
    auto *arg = env[optArgName];
    if (!arg)
        arg = env[argNo];
    return arg;
}

class Utilities
{
public:
    static void Print_LibFunc(Object &env)
    {
        int i = 0;
        while (env[i])
        {
            if (env[i]->GetType() == Value::ObjectType)
            {
                // PRINT_BLUE_LINE(ObjectToString(env[i]->ToObject_NoConst(), "", ""));
                NORMAL_PRINT_LINE(ObjectToString(env[i]->ToObject_NoConst(), "", "")); // If colors bug the terminal
            }

            else
            {
                // PRINT_BLUE_LINE(env[i]->Stringify());
                NORMAL_PRINT_LINE(env[i]->Stringify()); // If colors bug the terminal
            }

            i++;
        }
    }

    static void Typeof_Libfunc(Object &env)
    {
        // obj.Debug_PrintChildren();
        std::string retval = "";
        if (env[0]->GetType() == Value::UndefType)
            retval = "Undefined";
        else if (env[0]->GetType() == Value::NumberType)
            retval = "Number";
        else if (env[0]->GetType() == Value::BooleanType)
            retval = "Boolean";
        else if (env[0]->GetType() == Value::StringType)
            retval = "String";
        else if (env[0]->GetType() == Value::NilType)
            retval = "Nill";
        else if (env[0]->GetType() == Value::ObjectType)
            retval = "Object";
        else if (env[0]->GetType() == Value::ProgramFunctionType)
            retval = "ProgramFunction";
        else if (env[0]->GetType() == Value::LibraryFunctionType)
            retval = "LibFunction";
        else
            assert(false);
        env.Set(RETVAL_RESERVED_FIELD, Value(retval));
    }

    static void ObjectKeys_Libfunc(Object &env)
    {
        Object *keys = new Object();
        int i = 0;
        for (auto iter = env[0]->ToObject_NoConst()->children.begin(); iter != env[0]->ToObject_NoConst()->children.end(); ++iter)
        {
            keys->Set(i++, Value(iter->first));
        }
        env.Set(RETVAL_RESERVED_FIELD, Value(*keys));
    }

    static void ObjectSize_Libfunc(Object &env)
    {
        if (env[0]->GetType() != Value::ObjectType)
        {
            env.Set(RETVAL_RESERVED_FIELD, Value((double)1));
            return;
        }
        env.Set(RETVAL_RESERVED_FIELD, Value((double)env[0]->ToObject_NoConst()->children.size()));
    }

    static void GetInput_Libfunc(Object &env)
    {
        std::string input;
        std::cin >> input;
        if (StringIsNumber(input))
        {
            env.Set(RETVAL_RESERVED_FIELD, Value(std::stod(input)));
        }
        else
        {
            env.Set(RETVAL_RESERVED_FIELD, Value(input));
        }
    }

    static void ToInt_Libfunc(Object &env)
    {
        env.Set(RETVAL_RESERVED_FIELD, Value((double)(int)env[0]->ToNumber()));
    }

    static void Eval_Libfunc(Object &env)
    {
        auto& evalScope = LANG.PopScopeSpace();
        auto code = GetArgument(env, 0, "code")->ToString();
        
        // Check if code is just an expr, or a stmt/stmtlist
        if(code.find(";") == std::string::npos)
            code = "@;" + code;

        auto* ast = LANG.Parse(code);
        
        if (!ast)
        {
            LANG.Error("eval('" + code + "'): parse error");
            return;
        }
        ast->IncRefCounter();
        // ast->Set(PARENT_FIELD, LANG.GetEvalParent());
        // auto error = LANG.ValidateCode(*ast);
        // if (!error.empty())
        // {
        //     LANG.Error("eval('" + code + "'): " + error);
        //     return;
        // }
        auto val = LANG.Eval(*ast);
        ast->DecRefCounter();
        LANG.PushScopeSpace(evalScope);
        env.Set(RETVAL_RESERVED_FIELD, val);
    }

    static bool IsAstLeaf(Object &obj){
        std::cout << "IS LEAF: " << obj["$type"]->ToString() << std::endl;
        for(auto& it : obj.children){
            if(
                it.second.GetType() == Value::ObjectType &&
                (*it.second.ToObject_NoConst())[PARENT_FIELD] != nullptr
            )
            {
                std::cout << "~~~~~~~" << std::endl;
                it.second.ToObject_NoConst()->Debug_PrintChildren();
                return false;
            }
        }
        return true;
    }

private:
    static bool StringIsNumber(const std::string &str)
    {
        bool isNumber = true;
        bool isDecimal = false;

        for (std::string::size_type i = 0; i < str.size(); i++)
        {
            if (!isdigit(str[i]))
                isNumber = false;
            if (i == 0 && str[i] == '-')
                isNumber = true;

            if (str[i] == '.' && i > 0)
                if (isDecimal)
                    return false;
                else
                {
                    isDecimal = true;
                    isNumber = true;
                }

            if (isNumber == false)
                return false;
        }
        return true;
    }
};