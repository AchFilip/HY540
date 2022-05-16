#pragma once
#include <functional>
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
    ValueStack *envStack;

    struct BreakException
    {
    };
    struct ContinueException
    {
    };

    const Value Eval(Object &node)
    {
        return dispatcher->Eval(node);
    }
    const Value EvalStmts(Object &node)
    {
        node.Debug_PrintChildren();
        if (node[AST_TAG_STMTS]->GetType() != Value::NilType)
        {
            Eval(*node[AST_TAG_STMTS]->ToObject_NoConst());
            Eval(*node[AST_TAG_STMT]->ToObject_NoConst());
        }
        else
        {
            Eval(*node[AST_TAG_STMT]->ToObject_NoConst());
        }
    }
    const Value EvalStmt(Object &node)
    {
        node.Debug_PrintChildren();
        std::string stmtType;
        if (node[AST_TAG_EXPR])
            stmtType = AST_TAG_EXPR;
        else if (node[AST_TAG_IF])
            stmtType = AST_TAG_IF;
        else if (node[AST_TAG_WHILE])
            stmtType = AST_TAG_WHILE;
        else if (node[AST_TAG_FOR])
            stmtType = AST_TAG_FOR;
        else if (node[AST_TAG_RETURNSTMT])
            stmtType = AST_TAG_RETURNSTMT;
        else if (node[AST_TAG_BREAK])
            stmtType = AST_TAG_BREAK;
        else if (node[AST_TAG_CONTINUE])
            stmtType = AST_TAG_CONTINUE;
        else if (node[AST_TAG_BLOCK])
            stmtType = AST_TAG_BLOCK;
        else if (node[AST_TAG_FUNCDEF])
            stmtType = AST_TAG_FUNCDEF;

        return Eval(*node[stmtType]->ToObject_NoConst());
    }
    const Value EvalExpr(Object &node)
    {
        node.Debug_PrintChildren();

        if (node[AST_TAG_ASSIGNEXPR])
            return Eval(*node[AST_TAG_ASSIGNEXPR]->ToObject_NoConst());
        else if (node[AST_TAG_TERM])
            return Eval(*node[AST_TAG_TERM]->ToObject_NoConst());
        else if (node[AST_TAG_EXPR_LEFT] && node[AST_TAG_EXPR_RIGHT])
        {
            // Evaluate left and right expr. Explicitly use EvalExpr() instead of Eval().
            Value leftExpr = EvalExpr(*node[AST_TAG_EXPR_LEFT]->ToObject_NoConst());
            Value rightExpr = EvalExpr(*node[AST_TAG_EXPR_RIGHT]->ToObject_NoConst());

            // Do operation based on disambiguation
            std::string operatorStr = node[AST_TAG_DISAMBIGUATE_OBJECT]->ToString();
            if (operatorStr == "+")
                return leftExpr + rightExpr;
            else if (operatorStr == "-")
                return leftExpr - rightExpr;
            else if (operatorStr == "*")
                return leftExpr * rightExpr;
            else if (operatorStr == "/")
                return leftExpr / rightExpr;
            else if (operatorStr == "%")
                return leftExpr % rightExpr;
            else if (operatorStr == "==")
                return leftExpr == rightExpr;
            else if (operatorStr == "!=")
                return leftExpr != rightExpr;
            else if (operatorStr == ">")
                return leftExpr > rightExpr;
            else if (operatorStr == ">=")
                return leftExpr >= rightExpr;
            else if (operatorStr == "<")
                return leftExpr < rightExpr;
            else if (operatorStr == "<=")
                return leftExpr <= rightExpr;
            else if (operatorStr == "&&")
                return leftExpr && rightExpr;
            else if (operatorStr == "||")
                return leftExpr || rightExpr;
            else
                assert(false && "This kind of operator has not been supported yet");
        }
    }
    const Value EvalTerm(Object &node)
    {
        node.Debug_PrintChildren();

        std::string termType;
        if (node[AST_TAG_PRIMARY])
            termType = AST_TAG_PRIMARY;
        else if (node[AST_TAG_EXPR])
        {
            termType = AST_TAG_EXPR;
            // need to check -EXPR case
        }
        else if (node[AST_TAG_LVALUE])
        {
            // eval the lvalue and then incr/decr
            std::string operatorStr = node[AST_TAG_DISAMBIGUATE_OBJECT]->ToString();
            if (operatorStr == "notlvalue"){
                Value lvalue = Eval(*node[AST_TAG_LVALUE]->ToObject_NoConst());
                return !lvalue;
            }
            else if(operatorStr == "lvalue++"){
                Value &lvalue = const_cast<Value &>(EvalLvalue(*node[AST_TAG_LVALUE]->ToObject_NoConst(), false));
                Value originalValue = lvalue;
                lvalue = lvalue + Value(1.0);
                return originalValue;
            }
            else if (operatorStr == "++lvalue")
            {
                Value &lvalue = const_cast<Value &>(EvalLvalue(*node[AST_TAG_LVALUE]->ToObject_NoConst(), false));
                lvalue = lvalue + Value(1.0);
                return lvalue;
            }
            else if(operatorStr == "lvalue--"){
                Value &lvalue = const_cast<Value &>(EvalLvalue(*node[AST_TAG_LVALUE]->ToObject_NoConst(), false));
                Value originalValue = lvalue;
                lvalue = lvalue - Value(1.0);
                return originalValue;
            }
            else if (operatorStr == "--lvalue")
            {
                Value &lvalue = const_cast<Value &>(EvalLvalue(*node[AST_TAG_LVALUE]->ToObject_NoConst(), false));
                lvalue = lvalue - Value(1.0);
                return lvalue;
            }
        }
        else
            assert(false && "Not implemented yet for other types");

        return Eval(*node[termType]->ToObject_NoConst());
    }
    const Value EvalAssignexpr(Object &node)
    {
        node.Debug_PrintChildren();

        // Eval left and right part of assignment
        // TODO: differentiate between lvalue and rvalue evals by explicite calls.
        Value &lvalue = const_cast<Value &>(EvalLvalue(*node[AST_TAG_LVALUE]->ToObject_NoConst(), true));
        Value expr = Eval(*node[AST_TAG_EXPR]->ToObject_NoConst()); // Rvalue eval
        lvalue = expr; // Do you believe in god son?
        GetCurrentScope().Debug_PrintChildren();
    }
    const Value EvalPrimary(Object &node)
    {
        node.Debug_PrintChildren();

        if (node[AST_TAG_LVALUE])
        {
            return Eval(*node[AST_TAG_LVALUE]->ToObject_NoConst());
        }
        else if (node[AST_TAG_CONST])
        {
            return *node[AST_TAG_CONST];
        }
        else if (node[AST_TAG_OBJECTDEF])
        {
            return Eval(*node[AST_TAG_OBJECTDEF]->ToObject_NoConst());
        }
        else
            assert(false && "Not implmeneted yet for other types");
    }
    const Value EvalRvalue(Object &node)
    {
        node.Debug_PrintChildren();

        if (node[AST_TAG_ID])
        {
            // Check type of ID access
            std::string id = node[AST_TAG_ID]->ToString();
            std::string disambiguate = node[AST_TAG_DISAMBIGUATE_OBJECT]->ToString();

            if (disambiguate == "id")
            {
                const Value *lookup = ScopeLookup(GetCurrentScope(), id);
                if (lookup)
                {
                    std::cout << "Lookup Rvalue: found " << lookup->Stringify() << std::endl;
                    return *lookup;
                }
                else
                {
                    assert(false && "Nevermind this point, we will deal with it later");
                    return Value();
                }
            }
            else if (disambiguate == "local id")
            {
                const Value *lookup = ScopeLookup(GetCurrentScope(), LOCAL_SCOPE_KEY, id);
                if (lookup)
                    std::cout << "Lookup result: found" << std::endl;
                else
                    std::cout << "Lookup result: found" << std::endl;
                ; // TODO
            }
            else if (disambiguate == "doubledots id")
            {
                // TODO
            }
        }
        else
            assert(false && "Not implemented yet");
    }
    const Value EvalMember(Object &node)
    {
    }
    const Value EvalCall(Object &node)
    {
    }
    const Value EvalCallSuffix(Object &node)
    {
    }
    const Value EvalNormCall(Object &node)
    {
    }
    const Value EvalMethodCall(Object &node)
    {
    }
    const Value EvalElist(Object &node)
    {
    }
    const Value EvalObjectDef(Object &node)
    {
        node.Debug_PrintChildren();

        // TODO: use returned values of eval to create the object;
        if (node[AST_TAG_ELIST])
        {
            return Eval(*(node[AST_TAG_ELIST]->ToObject_NoConst()));
        }
        else if (node[AST_TAG_INDEXED])
        {
            return Eval(*(node[AST_TAG_INDEXED]->ToObject_NoConst()));
        }
        else
            assert(false && "Invalid ObjectDef node");
    }
    const Value EvalIndexed(Object &node)
    {
    }
    const Value EvalIndexedElem(Object &node)
    {
    }
    const Value EvalBlock(Object &node)
    {
        // Push new nested scope
        PushNested();
        // Eval code inside of block
        Eval(*node[AST_TAG_STMTS]->ToObject_NoConst());
        // Make sure that scopes have been restored
        auto &currScope = GetCurrentScope();
        bool shouldSliceOuter = currScope[PREVIOUS_SCOPE_KEY] != nullptr;
        if (shouldSliceOuter)
            while ((*PopScope())[PREVIOUS_SCOPE_KEY]) // Cringe fast code
            {
                // no-op (intentionally empty)
            }
        assert(GetCurrentScope()[OUTER_SCOPE_KEY] != nullptr);
        PopScope();
        if (shouldSliceOuter)
        {
            PushSlice();
        }
    }
    const Value EvalId(Object &node)
    {
    }
    const Value EvalFuncDef(Object &node)
    {
    }
    const Value EvalConst(Object &node)
    {
        assert(false && "Const shouldnt be evaluated, because you cant pass an object as a Const param. All consts are Values");
    }
    const Value EvalIdlist(Object &node)
    {
    }
    const Value EvalBreak(Object &node) { throw BreakException(); }
    const Value EvalContinue(Object &node) { throw ContinueException(); }
    const Value EvalWhile(Object &node)
    {
        while (dispatcher->Eval(*node[AST_TAG_WHILE_COND]->ToObject_NoConst()))
            try
            {
                dispatcher->Eval(*node[AST_TAG_WHILE_STMT]->ToObject_NoConst());
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
    const Value EvalFor(Object &node)
    {
    }
    const Value EvalIf(Object &node)
    {
        if (dispatcher->Eval(*node[AST_TAG_EXPR]->ToObject_NoConst()))
            dispatcher->Eval(*node[AST_TAG_IF_STMT]->ToObject_NoConst());
        else if (auto *elseStmt = node[AST_TAG_ELSE_STMT])
            dispatcher->Eval(*elseStmt->ToObject_NoConst());
        return _NIL_;
    }
    const Value EvalReturn(Object &node)
    {
    }
    const Value EvalObjGet(Object &node)
    {
        return *node["$value"];
    }
    const Value EvalObjSet(Object &node)
    {
    }
    const Value EvalObjSetWithValue(Object &node, const Value &value)
    {
    }
    const Value &EvalLvalue(Object &node, bool allowDefinition = false)
    {
        node.Debug_PrintChildren();

        if (node[AST_TAG_ID])
        {
            // Check type of ID access
            std::string id = node[AST_TAG_ID]->ToString();
            std::string disambiguate = node[AST_TAG_DISAMBIGUATE_OBJECT]->ToString();

            if (disambiguate == "id")
            {
                const Value *lookup = ScopeLookup(GetCurrentScope(), id);
                if (lookup)
                {
                    std::cout << "Lookup Lvalue: found " << lookup->Stringify() << std::endl;
                    return *lookup;
                }
                else if(allowDefinition)
                {
                    std::cout << "Lookup Lvalue: not_found" << std::endl;
                    auto &scope = GetCurrentScope();
                    scope.Set(id, *(new Value()));
                    return *(scope[id]);
                }
                else
                    assert(false && "Lvalue definition is not enabled and variable was not found");
            }
            else if (disambiguate == "local id")
            {
                const Value *lookup = ScopeLookup(GetCurrentScope(), LOCAL_SCOPE_KEY, id);
                if (lookup)
                    std::cout << "Lookup result: found" << std::endl;
                else
                    std::cout << "Lookup result: found" << std::endl;
                ; // TODO
            }
            else if (disambiguate == "doubledots id")
            {
                // TODO
            }
        }
        else
            assert(false && "Not implemented yet");
    }

    // Environment Handling Methods
    Object &GetCurrentScope()
    {
        return *envStack->Top().ToObject_NoConst()->children[TAIL_SCOPE_KEY].ToObject_NoConst();
    }
    void SetCurrentScope(Value *scope)
    {
        Object *obj = envStack->Top().ToObject_NoConst();
        envStack->Top().ToObject_NoConst()->GetAndRemove(TAIL_SCOPE_KEY);
        envStack->Top().ToObject_NoConst()->Set(TAIL_SCOPE_KEY, *scope);
    }
    Object &PushNewScope()
    {
        Object *newScope = new Object();
        SetCurrentScope(new Value(*newScope));
        return *newScope;
    }
    Object *PopScope()
    {
        // Assert something for antonis
        std::string parentKey = (GetCurrentScope().children.find(OUTER_SCOPE_KEY) != GetCurrentScope().children.end())
                                    ? OUTER_SCOPE_KEY
                                    : PREVIOUS_SCOPE_KEY;
        SetCurrentScope(new Value(GetCurrentScope().children[parentKey]));
        // Assert something for antonis
    }
    void PushSlice()
    {
        Object &previous = GetCurrentScope();
        Object &newScope = PushNewScope();
        newScope.Set(std::string(PREVIOUS_SCOPE_KEY), previous);
    }
    void PushNested()
    {
        Object &outer = GetCurrentScope();
        Object &newScope = PushNewScope();
        newScope.Set(std::string(OUTER_SCOPE_KEY), outer);
    }
    void PushScopeSpace()
    {
        Object *newScopeSpace = new Object();
        Value *value = new Value(*newScopeSpace);
        envStack->Push(*value);
    }
    void PushScopeSpace(Object *closure)
    {
        PushScopeSpace();
        SetCurrentScope(new Value(closure));
        envStack->Top().ToObject_NoConst()->GetAndRemove(CLOSURE_SCOPE_KEY);
        envStack->Top().ToObject_NoConst()->Set(CLOSURE_SCOPE_KEY, new Value(closure));
    }
    Value *PopScopeSpace()
    {
        envStack->Pop();
    }

    struct InvalidScopeValueException
    {
        std::string key;
    };
    const Value *ScopeLookup(const Object &scope, const std::string &id)
    {
        if (auto *val = scope[id])
            return val;
        else if (val = ScopeLookup(scope, LOCAL_SCOPE_KEY, id))
            return val;
        else if (val = ScopeLookup(scope, PREVIOUS_SCOPE_KEY, id))
            return val;
        else if (val = ScopeLookup(scope, OUTER_SCOPE_KEY, id))
            return val;
        else
            return nullptr;
    }
    const Value *ScopeLookup(const Object &from, const std::string &scopeKey, const std::string &id)
    {
        if (auto *scope = from[scopeKey])
            if (scope->GetType() != Value::ObjectType)
                throw InvalidScopeValueException{scopeKey};
            else if (auto *val = ScopeLookup(*scope->ToObject(), id))
                return val;
        return nullptr;
    }

    void Install(void)
    {
        dispatcher->Install(AST_TAG_STMTS, [this](Object &node)
                            { return EvalStmts(node); });
        dispatcher->Install(AST_TAG_STMT, [this](Object &node)
                            { return EvalStmt(node); });
        dispatcher->Install(AST_TAG_EXPR, [this](Object &node)
                            { return EvalExpr(node); });
        dispatcher->Install(AST_TAG_IF, [this](Object &node)
                            { return EvalIf(node); });
        dispatcher->Install(AST_TAG_WHILE, [this](Object &node)
                            { return EvalWhile(node); });
        dispatcher->Install(AST_TAG_FOR, [this](Object &node)
                            { return EvalFor(node); });
        dispatcher->Install(AST_TAG_RETURNSTMT, [this](Object &node)
                            { return EvalReturn(node); });
        dispatcher->Install(AST_TAG_BREAK, [this](Object &node)
                            { return EvalBreak(node); });
        dispatcher->Install(AST_TAG_CONTINUE, [this](Object &node)
                            { return EvalContinue(node); });
        dispatcher->Install(AST_TAG_BLOCK, [this](Object &node)
                            { return EvalBlock(node); });
        dispatcher->Install(AST_TAG_FUNCDEF, [this](Object &node)
                            { return EvalFuncDef(node); });
        dispatcher->Install(AST_TAG_ASSIGNEXPR, [this](Object &node)
                            { return EvalAssignexpr(node); });
        dispatcher->Install(AST_TAG_LVALUE, [this](Object &node)
                            { return EvalRvalue(node); });
        dispatcher->Install(AST_TAG_TERM, [this](Object &node)
                            { return EvalTerm(node); });
        dispatcher->Install(AST_TAG_PRIMARY, [this](Object &node)
                            { return EvalPrimary(node); });
        dispatcher->Install(AST_TAG_CONST, [this](Object &node)
                            { return EvalConst(node); });
        dispatcher->Install(AST_TAG_OBJECTDEF, [this](Object &node)
                            { return EvalObjectDef(node); });
        dispatcher->Install(AST_TAG_ELIST, [this](Object &node)
                            { return EvalElist(node); });
        dispatcher->Install(AST_TAG_INDEXED, [this](Object &node)
                            { return EvalIndexed(node); });

        dispatcher->Install(AST_TAG_IF, [this](Object &node)
                            { return EvalIf(node); });
        dispatcher->Install(std::string(AST_TAG_WHILE_COND), [this](Object &node)
                            { return EvalExpr(node); });
        dispatcher->Install(std::string(AST_TAG_WHILE_STMT), [this](Object &node)
                            { return EvalStmt(node); });

        // ... all the rest ../
    }

public:
    Interpreter()
    {
        dispatcher = new EvalDispatcher();

        envStack = new ValueStack();
        PushScopeSpace();
        PushNewScope();
        envStack->Debug_Print();

        Install();
    }
    ~Interpreter()
    {
        delete dispatcher;
        delete envStack;
    }

    void StartProgram(Object &node)
    {
        Eval(node);
    }
};
