
#pragma once
#include "Object.h"
#include "TreeTags.h"

class TreeVisitor
{
public:
    virtual void VisitVarDecl(const Object &node) {}
    virtual void VisitStmts(const Object &node) {}
    virtual void VisitStmt(const Object &node) {}
    virtual void VisitExpr(const Object &node) {}
    virtual void VisitArithmeticExpr(const Object &node) {}
    virtual void VisitRelationalExpr(const Object &node) {}
    virtual void VisitAssignexpr(const Object &node) {}
    virtual void VisitTerm(const Object &node) {}
    virtual void VisitPrimary(const Object &node) {}
    virtual void VisitLvalue(const Object &node) {}
    virtual void VisitMember(const Object &node) {}
    virtual void VisitCall(const Object &node) {}
    virtual void VisitCallSuffix(const Object &node) {}
    virtual void VisitNormCall(const Object &node) {}
    virtual void VisitMethodCall(const Object &node) {}
    virtual void VisitElist(const Object &node) {}
    virtual void VisitObjectDef(const Object &node) {}
    virtual void VisitIndexed(const Object &node) {}
    virtual void VisitIndexedElem(const Object &node) {}
    virtual void VisitBlock(const Object &node) {}
    virtual void VisitId(const Object &node) {}
    virtual void VisitFuncDef(const Object &node) {}
    virtual void VisitConst(const Object &node) {}
    virtual void VisitNumber(const Object &node) {}
    virtual void VisitIdlist(const Object &node) {}
    virtual void VisitIf(const Object &node) {}
    virtual void VisitWhile(const Object &node) {}
    virtual void VisitFor(const Object &node) {}
    virtual void VisitReturn(const Object &node) {}
    virtual void VisitBreak(const Object &node) {}
    virtual void VisitContinue(const Object &node) {}
    virtual void VisitQuasiQuotes(const Object &node) {}
    virtual void VisitEscape(const Object &node) {}
    virtual void VisitInline(const Object &node) {}
    virtual void VisitEscapeFunctionArgs(const Object &node) {}
    virtual void VisitEscapeFunctionId(const Object &node) {}

    virtual TreeVisitor *Clone(void) const = 0;
    TreeVisitor(void) = default;
    TreeVisitor(const TreeVisitor &) = default;
};