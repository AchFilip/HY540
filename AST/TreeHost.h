//#include <functional>
#pragma once
#include "TreeVisitor.h"
#include "./Value.h"
#include <map>
#include <assert.h>

class TreeHost
{
private:
    using Acceptor = std::function<void(const Object &)>;
    using Acceptors = std::map<std::string, Acceptor>;

    Acceptors acceptors;
    TreeVisitor *visitor = nullptr;

    void Accept(const Object &node)
    {
        std::cout << "type: " << node[AST_TAG_TYPE_KEY]->ToString() << std::endl;
        acceptors[node[AST_TAG_TYPE_KEY]->ToString()](node);
    }

    void AcceptStmt(const Object &node)
    {
        if (node[AST_TAG_EXPR] != nullptr)
            Accept(*node[AST_TAG_EXPR]->ToObject());
        else if (node[AST_TAG_IF] != nullptr)
            Accept(*node[AST_TAG_IF]->ToObject());
        else if (node[AST_TAG_WHILE] != nullptr)
            Accept(*node[AST_TAG_WHILE]->ToObject());
        else if (node[AST_TAG_FOR] != nullptr)
            Accept(*node[AST_TAG_FOR]->ToObject());
        else if (node[AST_TAG_RETURNSTMT] != nullptr && node[AST_TAG_RETURNSTMT]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_RETURNSTMT]->ToObject());
        else if (node[AST_TAG_BREAK] != nullptr)
            Accept(*node[AST_TAG_BREAK]->ToObject());
        else if (node[AST_TAG_CONTINUE] != nullptr)
            Accept(*node[AST_TAG_CONTINUE]->ToObject());
        else if (node[AST_TAG_BLOCK] != nullptr)
            Accept(*node[AST_TAG_BLOCK]->ToObject());
        else if (node[AST_TAG_FUNCDEF] != nullptr)
            Accept(*node[AST_TAG_FUNCDEF]->ToObject());
        visitor->VisitStmt(node);
    }
    void AcceptExpr(const Object &node)
    {
        if (node[AST_TAG_ASSIGNEXPR] != nullptr)
            Accept(*node[AST_TAG_ASSIGNEXPR]->ToObject());
        else if (node[AST_TAG_TERM] != nullptr)
            Accept(*node[AST_TAG_TERM]->ToObject());
        else
        {
            Accept(*node[AST_TAG_EXPR_LEFT]->ToObject());
            Accept(*node[AST_TAG_EXPR_RIGHT]->ToObject());
        }
        visitor->VisitExpr(node);
    }
    void AcceptTerm(const Object &node)
    {
        if (node[AST_TAG_EXPR] != nullptr)
            Accept(*node[AST_TAG_EXPR]->ToObject());
        else if (node[AST_TAG_LVALUE] != nullptr)
            Accept(*node[AST_TAG_LVALUE]->ToObject());
        else if (node[AST_TAG_PRIMARY] != nullptr)
            Accept(*node[AST_TAG_PRIMARY]->ToObject());
        visitor->VisitTerm(node);
    }
    void AcceptAssignexpr(const Object &node)
    {
        Accept(*node[AST_TAG_LVALUE]->ToObject());
        Accept(*node[AST_TAG_EXPR]->ToObject());
        visitor->VisitAssignexpr(node);
    }
    void AcceptPrimary(const Object &node)
    {
        if (node[AST_TAG_LVALUE] != nullptr)
            Accept(*node[AST_TAG_LVALUE]->ToObject());
        else if (node[AST_TAG_CALL] != nullptr)
            Accept(*node[AST_TAG_CALL]->ToObject());
        else if (node[AST_TAG_OBJECTDEF] != nullptr)
            Accept(*node[AST_TAG_OBJECTDEF]->ToObject());
        else if (node[AST_TAG_FUNCDEF] != nullptr)
            Accept(*node[AST_TAG_FUNCDEF]->ToObject());
        else if (node[AST_TAG_CONST] != nullptr)
            ;
        visitor->VisitPrimary(node);
    }
    void AcceptLvalue(const Object &node)
    {
        if (node[AST_TAG_MEMBER] != nullptr)
            Accept(*node[AST_TAG_MEMBER]->ToObject());
        visitor->VisitLvalue(node);
    }
    void AcceptMember(const Object &node)
    {
        if (node[AST_TAG_ID] != nullptr)
            ;
        else if (node[AST_TAG_EXPR] != nullptr)
            Accept(*node[AST_TAG_EXPR]->ToObject());
        visitor->VisitMember(node);
    }
    void AcceptCall(const Object &node)
    {
        if (node[AST_TAG_CALL] != nullptr)
        {
            Accept(*node[AST_TAG_CALL]->ToObject());
            if (node[AST_TAG_ELIST]->GetType() != Value::NilType)
                Accept(*node[AST_TAG_ELIST]->ToObject());
        }
        else if (node[AST_TAG_LVALUE] != nullptr)
        {
            Accept(*node[AST_TAG_LVALUE]->ToObject());
            Accept(*node[AST_TAG_CALLSUFFIX]->ToObject());
        }
        else if (node[AST_TAG_FUNCDEF] != nullptr)
        {
            Accept(*node[AST_TAG_FUNCDEF]->ToObject());
            if (node[AST_TAG_ELIST]->GetType() != Value::NilType)
                Accept(*node[AST_TAG_ELIST]->ToObject());
        }
        visitor->VisitCall(node);
    }
    void AcceptCallSuffix(const Object &node)
    {
        if (node[AST_TAG_NORMCALL] != nullptr && node[AST_TAG_NORMCALL]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_NORMCALL]->ToObject());
        else if (node[AST_TAG_METHODCALL] != nullptr && node[AST_TAG_METHODCALL]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_METHODCALL]->ToObject());
        visitor->VisitCallSuffix(node);
    }
    void AcceptNormCall(const Object &node)
    {
        if (node[AST_TAG_ELIST]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_ELIST]->ToObject());
        visitor->VisitNormCall(node);
    }
    void AcceptMethodCall(const Object &node)
    {
        if (node[AST_TAG_ELIST]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_ELIST]->ToObject());
        visitor->VisitMethodCall(node);
    }
    void AcceptElist(const Object &node)
    {
        if (node[AST_TAG_ELIST] != nullptr && node[AST_TAG_ELIST]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_ELIST]->ToObject());
        Accept(*node[AST_TAG_EXPR]->ToObject());
        visitor->VisitElist(node);
    }
    void AcceptObjectDef(const Object &node)
    {
        if (node[AST_TAG_ELIST] != nullptr && node[AST_TAG_ELIST]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_ELIST]->ToObject());
        else if (node[AST_TAG_INDEXED] != nullptr)
            Accept(*node[AST_TAG_INDEXED]->ToObject());
        visitor->VisitObjectDef(node);
    }
    void AcceptIndexed(const Object &node)
    {
        if (node[AST_TAG_INDEXED] != nullptr)
        {
            Accept(*node[AST_TAG_INDEXED]->ToObject());
            Accept(*node[AST_TAG_INDEXEDELEM]->ToObject());
        }
        else
            Accept(*node[AST_TAG_INDEXEDELEM]->ToObject());
        visitor->VisitIndexed(node);
    }
    void AcceptIndexedElem(const Object &node)
    {
        Accept(*node[AST_TAG_EXPR_LEFT]->ToObject());
        Accept(*node[AST_TAG_EXPR_RIGHT]->ToObject());
        visitor->VisitIndexedElem(node);
    }
    void AcceptStmts(const Object &node)
    {
        if (node[AST_TAG_STMTS]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_STMTS]->ToObject());
        if (node[AST_TAG_STMT]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_STMT]->ToObject());
        visitor->VisitStmts(node);
    }
    void AcceptBlock(const Object &node)
    {
        if (node[AST_TAG_STMTS]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_STMTS]->ToObject());
        visitor->VisitBlock(node);
    }
    void AcceptBreak(const Object &node)
    {
        visitor->VisitBreak(node);
    }
    void AcceptContinue(const Object &node)
    {
        visitor->VisitContinue(node);
    }
    void AcceptId(const Object &node)
    {
        visitor->VisitId(node);
    }
    void AcceptFuncDef(const Object &node)
    {
        if (node[AST_TAG_IDLIST]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_IDLIST]->ToObject());
        if (node[AST_TAG_BLOCK]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_BLOCK]->ToObject());
        visitor->VisitFuncDef(node);
    }
    void AcceptConst(const Object &node)
    {
        visitor->VisitConst(node);
    }
    void AcceptIdlist(const Object &node)
    {
        if (node[AST_TAG_IDLIST] != nullptr && node[AST_TAG_IDLIST]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_IDLIST]->ToObject());
        visitor->VisitIdlist(node);
    }
    void AcceptIf(const Object &node)
    {
        Accept(*node[AST_TAG_EXPR]->ToObject());
        if (node[AST_TAG_IF_STMT]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_IF_STMT]->ToObject());
        if (node[AST_TAG_ELSE_STMT] != nullptr && node[AST_TAG_IF_STMT]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_ELSE_STMT]->ToObject());
        visitor->VisitIf(node);
    }
    void AcceptWhile(const Object &node)
    {
        Accept(*node[AST_TAG_WHILE_COND]->ToObject());
        if (node[AST_TAG_WHILE_STMT]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_WHILE_STMT]->ToObject());
        visitor->VisitWhile(node);
    }
    void AcceptForstmt(const Object &node)
    {
        if (node[AST_TAG_INIT]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_INIT]->ToObject());

        Accept(*node[AST_TAG_EXPR]->ToObject());

        if (node[AST_TAG_FORCOND]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_FORCOND]->ToObject());
        if (node[AST_TAG_STMT]->GetType() != Value::NilType)
            Accept(*node[AST_TAG_STMT]->ToObject());
        visitor->VisitFor(node);
    }
    void AcceptReturn(const Object &node)
    {
        if (node[AST_TAG_EXPR] != nullptr)
            Accept(*node[AST_TAG_EXPR]->ToObject());
        visitor->VisitReturn(node);
    }

    // ... all the rest tag-specific (node-type specific) accept methods ...
    void InstallAcceptor(const std::string &tag, const Acceptor &f)
    {
        acceptors[tag] = f;
    }

    void InstallAllAcceptors(void)
    {
        InstallAcceptor(AST_TAG_STMT, [this](const Object &node)
                        { AcceptStmt(node); });
        InstallAcceptor(AST_TAG_EXPR, [this](const Object &node)
                        { AcceptExpr(node); });
        InstallAcceptor(AST_TAG_TERM, [this](const Object &node)
                        { AcceptTerm(node); });
        InstallAcceptor(AST_TAG_ASSIGNEXPR, [this](const Object &node)
                        { AcceptAssignexpr(node); });
        InstallAcceptor(AST_TAG_PRIMARY, [this](const Object &node)
                        { AcceptPrimary(node); });
        InstallAcceptor(AST_TAG_LVALUE, [this](const Object &node)
                        { AcceptLvalue(node); });
        InstallAcceptor(AST_TAG_MEMBER, [this](const Object &node)
                        { AcceptMember(node); });
        InstallAcceptor(AST_TAG_CALL, [this](const Object &node)
                        { AcceptCall(node); });
        InstallAcceptor(AST_TAG_CALLSUFFIX, [this](const Object &node)
                        { AcceptCallSuffix(node); });
        InstallAcceptor(AST_TAG_NORMCALL, [this](const Object &node)
                        { AcceptNormCall(node); });
        InstallAcceptor(AST_TAG_METHODCALL, [this](const Object &node)
                        { AcceptMethodCall(node); });
        InstallAcceptor(AST_TAG_ELIST, [this](const Object &node)
                        { AcceptElist(node); });
        InstallAcceptor(AST_TAG_OBJECTDEF, [this](const Object &node)
                        { AcceptObjectDef(node); });
        InstallAcceptor(AST_TAG_INDEXED, [this](const Object &node)
                        { AcceptIndexed(node); });
        InstallAcceptor(AST_TAG_STMTS, [this](const Object &node)
                        { AcceptStmts(node); });
        InstallAcceptor(AST_TAG_BLOCK, [this](const Object &node)
                        { AcceptBlock(node); });
        InstallAcceptor(AST_TAG_ID, [this](const Object &node)
                        { AcceptId(node); });
        InstallAcceptor(AST_TAG_FUNCDEF, [this](const Object &node)
                        { AcceptFuncDef(node); });
        InstallAcceptor(AST_TAG_CONST, [this](const Object &node)
                        { AcceptConst(node); });
        InstallAcceptor(AST_TAG_IDLIST, [this](const Object &node)
                        { AcceptIdlist(node); });
        InstallAcceptor(AST_TAG_IF, [this](const Object &node)
                        { AcceptIf(node); });
        InstallAcceptor(AST_TAG_WHILE, [this](const Object &node)
                        { AcceptWhile(node); });
        InstallAcceptor(AST_TAG_FOR, [this](const Object &node)
                        { AcceptForstmt(node); });
        InstallAcceptor(AST_TAG_RETURNSTMT, [this](const Object &node)
                        { AcceptReturn(node); });
    }

public:
    TreeHost()
    {
        InstallAllAcceptors();
    }

    void Accept(TreeVisitor *_visitor, const Object &node) // post order visit
    {
        visitor = _visitor;
        Accept(node);
    }
};
