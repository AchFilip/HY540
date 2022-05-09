#pragma once
#include "./TreeTags.h"
#include "./Object.h"
#include "./Value.h"
#include "./ValueStack.h"
#include "./EvalDispatcher.h"
#include <iostream>

class Interpreter
{
private:
    EvalDispatcher *dispatcher;
    ValueStack *pain;

    const Value EvalStmt(Object &node)
    {
        if (node["$child"] != nullptr)
            dispatcher->Eval(*node["$child"]);
    }
    const Value EvalExpr(Object &node)
    {
        if (node["$child"] != nullptr)
            dispatcher->Eval(*node["$child"]);
        else
        {
            dispatcher->Eval(*node["$child1"]);
            dispatcher->Eval(*node["$child2"]);
        }
    }
    const Value EvalTerm(Object &node)
    {
        if (node["$child"] != nullptr)
            dispatcher->Eval(*node["$child"]);
        else
        {
            dispatcher->Eval(*node["$child1"]);
            dispatcher->Eval(*node["$child2"]);
        }
    }
    const Value EvalAssignexpr(Object &node)
    {
        dispatcher->Eval(*node["$lvalue"]);
        dispatcher->Eval(*node["$expr"]);
    }
    const Value EvalPrimary(Object &node)
    {
        dispatcher->Eval(*node["$child"]);
    }
    const Value EvalLvalue(Object &node)
    {
        dispatcher->Eval(*node["$child"]);
    }
    const Value EvalMember(Object &node)
    {
        dispatcher->Eval(*node["$child1"]);
        dispatcher->Eval(*node["$child2"]);
    }
    const Value EvalCall(Object &node)
    {
        dispatcher->Eval(*node["$child1"]);
        dispatcher->Eval(*node["$child2"]);
    }
    const Value EvalCallSuffix(Object &node)
    {
        dispatcher->Eval(*node["$child"]);
    }
    const Value EvalNormCall(Object &node)
    {
        dispatcher->Eval(*node["$child"]);
    }
    const Value EvalMethodCall(Object &node)
    {
        dispatcher->Eval(*node["$child1"]);
        dispatcher->Eval(*node["$child2"]);
    }
    const Value EvalElist(Object &node)
    {
        if (node["$child"] != nullptr)
            dispatcher->Eval(*node["$child"]);
        else if (node["$child1"] != nullptr)
        {
            dispatcher->Eval(*node["$child1"]);
            dispatcher->Eval(*node["$child2"]);
        }
    }
    const Value EvalObjectDef(Object &node)
    {
        dispatcher->Eval(*node["$child"]);
    }
    const Value EvalIndexed(Object &node)
    {
        if (node["$child"] != nullptr)
            dispatcher->Eval(*node["$child"]);
        else
        {
            dispatcher->Eval(*node["$child1"]);
            dispatcher->Eval(*node["$child2"]);
        }
    }
    const Value EvalIndexedElem(Object &node)
    {
        dispatcher->Eval(*node["$child1"]);
        dispatcher->Eval(*node["$child2"]);
    }
    const Value EvalStmts(Object &node)
    {
        if (node["$child1"] != nullptr)
        {
            dispatcher->Eval(*node["$child1"]);
            dispatcher->Eval(*node["$child2"]);
        }
    }
    const Value EvalBlock(Object &node)
    {
        dispatcher->Eval(*node["$child"]);
    }
    const Value EvalId(Object &node)
    {
    }
    const Value EvalFuncDef(Object &node)
    {
        if (node["$id"] != nullptr)
            dispatcher->Eval(*node["$id"]);
        dispatcher->Eval(*node["$idlist"]);
        dispatcher->Eval(*node["$block"]);
    }
    const Value EvalConst(Object &node)
    {
    }
    const Value EvalIdlist(Object &node)
    {
        if (node["$child"] != nullptr)
            dispatcher->Eval(*node["$child"]);
        else if (node["$child1"] != nullptr)
        {
            dispatcher->Eval(*node["$child1"]);
            dispatcher->Eval(*node["$child2"]);
        }
    }
    const Value EvalForstmt(Object &node)
    {
        dispatcher->Eval(*node["$init"]);
        dispatcher->Eval(*node["$cond"]);
        dispatcher->Eval(*node["$expr"]);
        dispatcher->Eval(*node["$stmt"]);
    }
    const Value EvalReturn(Object &node)
    {
    }

    const Value EvalIf(Object &node)
    {
        if (dispatcher->Eval(*node["$ifcond"]))
            dispatcher->Eval(*node["$ifstmt"]);
        else if (auto *elseStmt = node["$elsestmt"])
            dispatcher->Eval(*elseStmt);
        return _NIL_;
    }

    struct BreakException
    {
    };
    struct ContinueException
    {
    };

    const Value EvalBreak(Object &node) { throw BreakException(); }
    const Value EvalContinue(Object &node) { throw ContinueException(); }

    const Value EvalWhile(Object &node)
    {
        while (dispatcher->Eval(*node["$whilecond"]))
            try
            {
                dispatcher->Eval(*node["$whilestmt"]);
            }
            catch (const BreakException &)
            {
                break;
            }
            catch (const ContinueException &)
            {
                continue;
            } // redundant
        return _NIL_;
    }

    void Install(void)
    {
        dispatcher->Install(std::string(AST_TAG_IF), [this](Object &node)
                           { return EvalIf(node); });
        dispatcher->Install(std::string(AST_TAG_IF_COND), [this](Object &node)
                           { return EvalExpr(node); });
        dispatcher->Install(std::string(AST_TAG_IF_STMT), [this](Object &node){
             return EvalStmt(node); 
        });

        dispatcher->Install(std::string(AST_TAG_WHILE_COND), [this](Object &node)
                           { return EvalExpr(node); });
        dispatcher->Install(std::string(AST_TAG_WHILE_STMT), [this](Object &node)
                           { return EvalStmt(node); });

        // ... all the rest ../
    }

    // Environment Handling Methods
    Object *GetCurrentScope()
    {
        return pain->Top().ToObject_NoConst()->children[TAIL_SCOPE_KEY];
    }
    void SetCurrentScope(Value *scope)
    {
        pain->Top().ToObject_NoConst()->children[TAIL_SCOPE_KEY] = scope->ToObject_NoConst();
    }
    Object *PushNewScope()
    {
        Object *newScope = new Object();
        SetCurrentScope(new Value(newScope));
        return newScope;
    }
    Object *PopScope()
    {
        // Assert something for antonis
        std::string parentKey = (GetCurrentScope()->children.find(OUTER_SCOPE_KEY) != GetCurrentScope()->children.end())
                                    ? OUTER_SCOPE_KEY : PREVIOUS_SCOPE_KEY;
        SetCurrentScope(new Value(GetCurrentScope()->children[parentKey]));
        // Assert something for antonis
    }
    void PushSlice()
    {
        Object *previous = GetCurrentScope();
        Object *newScope = PushNewScope();
        newScope->Set(std::string(PREVIOUS_SCOPE_KEY), *previous);
    }
    void PushNested()
    {
        Object *outer = GetCurrentScope();
        Object *newScope = PushNewScope();
        newScope->Set(std::string(OUTER_SCOPE_KEY), *outer);
    }
    void PushScopeSpace()
    {
        Object *newScopeSpace = new Object();
        pain->Push(new Value(newScopeSpace));
    }
    void PushScopeSpace(Object *closure)
    {
        PushScopeSpace();
        SetCurrentScope(new Value(closure));
        pain->Top().ToObject_NoConst()->children[CLOSURE_SCOPE_KEY] = closure;
    }
    Value *PopScopeSpace()
    {
        pain->Pop();
    }

public:
    Interpreter()
    {
        dispatcher = new EvalDispatcher();
        pain = new ValueStack();
    }
};
