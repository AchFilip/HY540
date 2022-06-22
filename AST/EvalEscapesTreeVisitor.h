#pragma once
#include "./Object.h"
#include "./TreeVisitor.h"
#include "./ValueStack.h"
#include <iostream>
#include "./EvalDispatcher.h"

class EvalEscapesTreeVisitor final : public TreeVisitor
{
private:
    ValueStack valueStack;
    EvalDispatcher *dispatcher;
    bool isRemoval = false;

public:
    virtual TreeVisitor *Clone(void) const { return nullptr; };
    EvalEscapesTreeVisitor(void) = default;
    EvalEscapesTreeVisitor(const EvalEscapesTreeVisitor &) = default;
    void SetDispatcher(EvalDispatcher* d){
        this->dispatcher = d;
    }

#define IMPL_VISIT(type) \
    virtual void Visit##type(const Object &node) override { if(node[AST_TAG_ESCAPE] != nullptr) {this->dispatcher->Eval(*(node[AST_TAG_ESCAPE]->ToObject_NoConst())); }}
    IMPL_VISIT(VarDecl)
    IMPL_VISIT(Stmts)
    IMPL_VISIT(Stmt)
    IMPL_VISIT(Expr)
    IMPL_VISIT(ArithmeticExpr)
    IMPL_VISIT(RelationalExpr)
    IMPL_VISIT(Assignexpr)
    IMPL_VISIT(Term)
    IMPL_VISIT(Primary)
    IMPL_VISIT(Lvalue)
    IMPL_VISIT(Member)
    IMPL_VISIT(Call)
    IMPL_VISIT(CallSuffix)
    IMPL_VISIT(NormCall)
    IMPL_VISIT(MethodCall)
    IMPL_VISIT(Elist)
    IMPL_VISIT(ObjectDef)
    IMPL_VISIT(Indexed)
    IMPL_VISIT(IndexedElem)
    IMPL_VISIT(Block)
    IMPL_VISIT(Id)
    IMPL_VISIT(FuncDef)
    IMPL_VISIT(Const)
    IMPL_VISIT(Number)
    IMPL_VISIT(Idlist)
    IMPL_VISIT(If)
    IMPL_VISIT(While)
    IMPL_VISIT(For)
    IMPL_VISIT(Return)
    IMPL_VISIT(Break)
    IMPL_VISIT(Continue)
    IMPL_VISIT(QuasiQuotes)
    IMPL_VISIT(Escape)
    IMPL_VISIT(Inline)
#undef IMPL_VISIT

    void SetIsRemoval(bool v)
    {
        isRemoval = v;
    }
};