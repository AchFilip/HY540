#pragma once
#include "./Object.h"
#include "./Value.h"
#include "./EvalDispatcher.h"
#include <iostream>

class Interpreter
{
private:
    EvalDispatcher dispatcher;

    void EvalStmt(const Object &node)
    {
        if (node["$child"] != nullptr)
            Eval(*node["$child"]);
        visitor->VisitStmt(node);
    }
    const Value EvalExpr(const Object &node)
    {
        if (node["$child"] != nullptr)
            Eval(*node["$child"]);
        else
        {
            Eval(*node["$child1"]);
            Eval(*node["$child2"]);
        }
        visitor->VisitExpr(node);
    }
    const Value EvalTerm(const Object &node)
    {
        if (node["$child"] != nullptr)
            Eval(*node["$child"]);
        else
        {
            Eval(*node["$child1"]);
            Eval(*node["$child2"]);
        }
        visitor->VisitTerm(node);
    }
    const Value EvalAssignexpr(const Object &node)
    {
        Eval(*node["$lvalue"]);
        Eval(*node["$expr"]);
        visitor->VisitAssignexpr(node);
    }
    const Value EvalPrimary(const Object &node)
    {
        Eval(*node["$child"]);
        visitor->VisitPrimary(node);
    }
    const Value EvalLvalue(const Object &node)
    {
        Eval(*node["$child"]);
        visitor->VisitLvalue(node);
    }
    const Value EvalMember(const Object &node)
    {
        Eval(*node["$child1"]);
        Eval(*node["$child2"]);
        visitor->VisitMember(node);
    }
    const Value EvalCall(const Object &node)
    {
        Eval(*node["$child1"]);
        Eval(*node["$child2"]);
        visitor->VisitCall(node);
    }
    const Value EvalCallSuffix(const Object &node)
    {
        Eval(*node["$child"]);
        visitor->VisitCallSuffix(node);
    }
    const Value EvalNormCall(const Object &node)
    {
        Eval(*node["$child"]);
        visitor->VisitNormCall(node);
    }
    const Value EvalMethodCall(const Object &node)
    {
        Eval(*node["$child1"]);
        Eval(*node["$child2"]);
        visitor->VisitMethodCall(node);
    }
    const Value EvalElist(const Object &node)
    {
        if (node["$child"] != nullptr)
            Eval(*node["$child"]);
        else if (node["$child1"] != nullptr)
        {
            Eval(*node["$child1"]);
            Eval(*node["$child2"]);
        }
        visitor->VisitElist(node);
    }
    const Value EvalObjectDef(const Object &node)
    {
        Eval(*node["$child"]);
        visitor->VisitObjectDef(node);
    }
    const Value EvalIndexed(const Object &node)
    {
        if (node["$child"] != nullptr)
            Eval(*node["$child"]);
        else
        {
            Eval(*node["$child1"]);
            Eval(*node["$child2"]);
        }
        visitor->VisitIndexed(node);
    }
    const Value EvalIndexedElem(const Object &node)
    {
        Eval(*node["$child1"]);
        Eval(*node["$child2"]);
        visitor->VisitIndexedElem(node);
    }
    const Value EvalStmts(const Object &node)
    {
        if (node["$child1"] != nullptr)
        {
            Eval(*node["$child1"]);
            Eval(*node["$child2"]);
        }
        visitor->VisitStmts(node);
    }
    const Value EvalBlock(const Object &node)
    {
        Eval(*node["$child"]);
        visitor->VisitBlock(node);
    }
    const Value EvalId(const Object &node)
    {
        visitor->VisitId(node);
    }
    const Value EvalFuncDef(const Object &node)
    {
        if (node["$id"] != nullptr)
            Eval(*node["$id"]);
        Eval(*node["$idlist"]);
        Eval(*node["$block"]);
        visitor->VisitFuncDef(node);
    }
    const Value EvalConst(const Object &node)
    {
        visitor->VisitConst(node);
    }
    const Value EvalIdlist(const Object &node)
    {
        if (node["$child"] != nullptr)
            Eval(*node["$child"]);
        else if (node["$child1"] != nullptr)
        {
            Eval(*node["$child1"]);
            Eval(*node["$child2"]);
        }
        visitor->VisitIdlist(node);
    }
    const Value EvalForstmt(const Object &node)
    {
        Eval(*node["$init"]);
        Eval(*node["$cond"]);
        Eval(*node["$expr"]);
        Eval(*node["$stmt"]);
        visitor->VisitFor(node);
    }
    const Value EvalReturn(const Object &node)
    {
        visitor->VisitReturn(node);
    }

    const Value EvalIf(Object &node)
    {
        if (dispatcher.Eval(*node["$ifcond"])
            dispatcher.Eval(*node["$ifstmt"]);
        else if (auto *elseStmt = node["$elsestmt"])
            dispatcher.Eval(*elseStmt);
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
        while (dispatcher.Eval(*node["$whilecond"]))
            try
            {
                dispatcher.Eval(*node["$whilestmt"]);
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
        dispatcher.Install(AST_TAG_IF, [this](Object &node)
                           { return EvalIfElse(node); });
        dispatcher.Install(AST_TAG_IF_COND, [this](Object &node)
                           { return EvalExpr(node); });
        dispatcher.Install(AST_TAG_IF_STMT, [this](Object &node)
                           { return EvalStmt(node); });

        dispatcher.Install(AST_TAG_WHILE_COND, [this](Object &node)
                           { return Eval_Expr(node); });
        dispatcher.Install(AST_TAG_WHILE_STMT, [this](Object &node)
                           { return Eval_Stmt(node); });

        // ... all the rest ../
    }
};
