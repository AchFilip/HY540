
#pragma once
#include "Object.h"
#include "TreeTags.h"
#include "TreeVisitor.h"

#define PRINT_VALUE "$print"

class PrintTreeVisitor : public TreeVisitor
{
private:
    const std::string GetPrint(const Value *val)
    {
        return val->ToObject_NoConst()->GetAndRemove(PRINT_VALUE).ToString();
    }

    const std::string PrintStmts(const std::string &stmts, const std::string &stmt)
    {
        return std::string("--stmts\n\t--" + stmts + "\n\t--" + stmt + "\n");
    }
    const std::string PrintStmt(const std::string &expr)
    {
        return std::string("--stmt\n\t--" + expr + "\n");
    }
    const std::string PrintExpr(const std::string &expr, const std::string &r_expr)
    {
        std::string res = std::string("--expr\n\t--" + expr + "\n");
        if (!r_expr.empty())
        {
            res += std::string("\t--" + r_expr + "\n");
        }
        return res;
    }
    const std::string PrintAssignExpr(const std::string &lvalue, const std::string &expr)
    {
        return std::string("--assignexpr\n\t--" + lvalue + "\n\t--" + expr + "\n");
    }
    const std::string PrintPrimary(const std::string &expr)
    {
        return std::string("--primary\n\t--" + expr + "\n");
    }
    const std::string PrintLvalue(const std::string &expr)
    {
        return std::string("--lvalue\n\t--" + expr + "\n");
    }
    const std::string PrintMember(const std::string &lval, const std::string &expr)
    {
        return std::string("--member\n\t--" + lval + "\n\t--" + expr + "\n");
    }
    const std::string PrintCall(const std::string &expr1, const std::string &expr2)
    {
        return std::string("--call\n\t--" + expr1 + "\n\t--" + expr2 + "\n");
    }
    const std::string PrintCallSuffix(const std::string &expr)
    {
        return std::string("--callsuffix\n\t--" + expr + "\n");
    }
    const std::string PrintNormCall(const std::string &expr)
    {
        return std::string("--normcall\n\t--" + expr + "\n");
    }
    const std::string PrintMethodCall(const std::string &id, const std::string &elist)
    {
        return std::string("--methodcall\n\t--" + id + "\n\t--" + elist + "\n");
    }
    const std::string PrintElist(const std::string &elist, const std::string &expr)
    {
        if (!elist.empty())
        {
            return std::string("--elist\n\t--" + elist + "\n\t--" + expr + "\n");
        }
        else if (!expr.empty())
        {
            return std::string("--elist\n\t--" + expr + "\n");
        }
        else
        {
            return std::string("--elist\n");
        }
    }
    const std::string PrintObjectdef(const std::string &expr)
    {
        return std::string("--objectdef\n\t--" + expr + "\n");
    }
    const std::string PrintIndexed(const std::string &indexed, const std::string &indexedelem)
    {
        if (!indexed.empty())
            return std::string("--indexed\n\t--" + indexed + "\n\t--" + indexedelem + "\n");
        else
            return std::string("--indexed\n\t--" + indexedelem + "\n");
    }
    const std::string PrintIndexedElem(const std::string &expr_left, const std::string &expr_right)
    {
        return std::string("--indexedelem\n\t--" + expr_left + "\n\t--" + expr_right + "\n");
    }
    const std::string PrintBlock(const std::string &stmts)
    {
        return std::string("--block\n\t--" + stmts + "\n");
    }
    const std::string PrintFuncdef(const std::string &id,
                                   const std::string &idlist, const std::string &block)
    {
        if (!id.empty())
        {
            return std::string("--funcdef\n\t--" + id + "\n\t--" + idlist + "\n\t--" + block + "\n");
        }
        else
        {
            return std::string("--funcdef\n\t--" + idlist + "\n\t--" + block + "\n");
        }
    }
    const std::string PrintIdlist(const std::string &idlist, const std::string &id)
    {
        if (!idlist.empty())
        {
            return std::string("--idlist\n\t--" + idlist + "\n\t--" + id + "\n");
        }
        else if (!id.empty())
        {
            return std::string("--idlist\n\t--" + id + "\n");
        }
        else
        {
            return std::string("--idlist\n");
        }
    }
    const std::string PrintTerm(const std::string &expr)
    {
        return std::string("--term\n\t--" + expr + "\n");
    }
    const std::string PrintConst()
    {
        return std::string("--const\n");
    }
    const std::string PrintIf(const std::string &expr,
                              const std::string &ifstmt, const std::string &elsestmt)
    {
        if (elsestmt.empty())
            return std::string("--ifstmt\n\t--" + expr + "\n\t--" + ifstmt + "\n");
        else
        {
            return std::string("--ifstmt\n\t--" + expr + "\n\t--" + ifstmt + "\n\t--" + elsestmt + "\n");
        }
    }
    const std::string PrintWhile(const std::string &expr, const std::string &stmt)
    {
        return std::string("--while\n\t--" + expr + "\n\t--" + stmt + "\n");
    }
    const std::string PrintFor(const std::string &init, const std::string &expr,
                               const std::string &cond, const std::string &stmt)
    {
        return std::string("--for\n\t--" + init + "\n\t--" + expr + "\n\t--" + cond + "\n\t--" + stmt + "\n");
    }
    const std::string PrintIfElse(const std::string &expr, const std::string &stmt)
    {
        return std::string("--ifelsestmt\n\t--" + expr + "\n\t--" + stmt + "\n\t--" + stmt + "\n");
    }
    const std::string PrintReturn(const std::string &expr)
    {
        return std::string("--returnstmt\n\t--" + expr + "\n");
    }

public:
    virtual void VisitVarDecl(const Object &node) {}
    virtual void VisitStmts(const Object &node)
    {
        const_cast<Object &>(node).Set(PRINT_VALUE,
                                       PrintStmts(
                                           GetPrint(node[AST_TAG_STMTS]),
                                           GetPrint(node[AST_TAG_STMT])));
    }
    virtual void VisitStmt(const Object &node)
    {
        if (node[AST_TAG_EXPR])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintStmt(GetPrint(node[AST_TAG_EXPR])));
        }
        else if (node[AST_TAG_IF])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintStmt(GetPrint(node[AST_TAG_EXPR])));
        }
        else if (node[AST_TAG_WHILE])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintStmt(GetPrint(node[AST_TAG_EXPR])));
        }
        else if (node[AST_TAG_FOR])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintStmt(GetPrint(node[AST_TAG_EXPR])));
        }
        else if (node[AST_TAG_RETURNSTMT])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintStmt(GetPrint(node[AST_TAG_EXPR])));
        }
        else if (node[AST_TAG_BREAK])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintStmt(GetPrint(node[AST_TAG_EXPR])));
        }
        else if (node[AST_TAG_CONTINUE])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintStmt(GetPrint(node[AST_TAG_EXPR])));
        }
        else if (node[AST_TAG_BLOCK])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintStmt(GetPrint(node[AST_TAG_EXPR])));
        }
        else if (node[AST_TAG_FUNCDEF])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintStmt(GetPrint(node[AST_TAG_EXPR])));
        }
        else
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, "");
        }
    }
    virtual void VisitExpr(const Object &node)
    {
        if (node[AST_TAG_ASSIGNEXPR])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintExpr(GetPrint(node[AST_TAG_ASSIGNEXPR]), ""));
        }
        else if (node[AST_TAG_TERM])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintExpr(GetPrint(node[AST_TAG_TERM]), ""));
        }
        else
        {
            const_cast<Object &>(node).Set(PRINT_VALUE,
                                           PrintExpr(
                                               GetPrint(node[AST_TAG_EXPR]),
                                               GetPrint(node[AST_TAG_EXPR])));
        }
    }
    virtual void VisitArithmeticExpr(const Object &node) {}
    virtual void VisitRelationalExpr(const Object &node) {}
    virtual void VisitAssignxpr(const Object &node)
    {
        const_cast<Object &>(node).Set(PRINT_VALUE,
                                       PrintAssignExpr(
                                           GetPrint(node[AST_TAG_LVALUE]),
                                           GetPrint(node[AST_TAG_EXPR])));
    }
    virtual void VisitTerm(const Object &node)
    {
        if (node[AST_TAG_EXPR])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintTerm(GetPrint(node[AST_TAG_EXPR])));
        }
        else if (node[AST_TAG_LVALUE])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintTerm(GetPrint(node[AST_TAG_LVALUE])));
        }
        else
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintTerm(GetPrint(node[AST_TAG_PRIMARY])));
        }
    }
    virtual void VisitPrimary(const Object &node)
    {
        if (node[AST_TAG_LVALUE])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintPrimary(GetPrint(node[AST_TAG_LVALUE])));
        }
        else if (node[AST_TAG_CALL])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintPrimary(GetPrint(node[AST_TAG_CALL])));
        }
        else if (node[AST_TAG_OBJECTDEF])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintPrimary(GetPrint(node[AST_TAG_OBJECTDEF])));
        }
        else if (node[AST_TAG_FUNCDEF])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintPrimary(GetPrint(node[AST_TAG_FUNCDEF])));
        }
        else if (node[AST_TAG_CONST])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintPrimary(GetPrint(node[AST_TAG_CONST])));
        }
    }
    virtual void VisitLvalue(const Object &node)
    {
        if (node[AST_TAG_ID])
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintLvalue(GetPrint(node[AST_TAG_ID])));
        else if (node[AST_TAG_MEMBER])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintLvalue(GetPrint(node[AST_TAG_MEMBER])));
        }
    }
    virtual void VisitMember(const Object &node)
    {
        if (node[AST_TAG_LVALUE])
        {
            if (node[AST_TAG_ID])
            {
                const_cast<Object &>(node).Set(PRINT_VALUE, PrintMember(
                                                                GetPrint(node[AST_TAG_LVALUE]),
                                                                GetPrint(node[AST_TAG_ID])));
            }
            else
            {
                const_cast<Object &>(node).Set(PRINT_VALUE, PrintMember(
                                                                GetPrint(node[AST_TAG_LVALUE]),
                                                                GetPrint(node[AST_TAG_EXPR])));
            }
        }
        else if (node[AST_TAG_CALL])
        {
            if (node[AST_TAG_ID])
            {
                const_cast<Object &>(node).Set(PRINT_VALUE, PrintMember(
                                                                GetPrint(node[AST_TAG_CALL]),
                                                                GetPrint(node[AST_TAG_ID])));
            }
            else
            {
                const_cast<Object &>(node).Set(PRINT_VALUE, PrintMember(
                                                                GetPrint(node[AST_TAG_CALL]),
                                                                GetPrint(node[AST_TAG_EXPR])));
            }
        }
    }
    virtual void VisitCall(const Object &node)
    {
        if (node[AST_TAG_CALL])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintCall(
                                                            GetPrint(node[AST_TAG_CALL]),
                                                            GetPrint(node[AST_TAG_ELIST])));
        }
        else if (node[AST_TAG_LVALUE])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintCall(
                                                            GetPrint(node[AST_TAG_LVALUE]),
                                                            GetPrint(node[AST_TAG_CALLSUFFIX])));
        }
        else if (node[AST_TAG_FUNCDEF])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintCall(
                                                            GetPrint(node[AST_TAG_FUNCDEF]),
                                                            GetPrint(node[AST_TAG_ELIST])));
        }
    }
    virtual void VisitCallSuffix(const Object &node)
    {
        if (node[AST_TAG_NORMCALL])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintCallSuffix(GetPrint(node[AST_TAG_NORMCALL])));
        }
        else if (node[AST_TAG_METHODCALL])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintCallSuffix(GetPrint(node[AST_TAG_METHODCALL])));
        }
    }
    virtual void VisitNormCall(const Object &node)
    {
        const_cast<Object &>(node).Set(PRINT_VALUE, PrintNormCall(GetPrint(node[AST_TAG_ELIST])));
    }
    virtual void VisitMethodCall(const Object &node)
    {
        const_cast<Object &>(node).Set(PRINT_VALUE, PrintMethodCall(
                                                        GetPrint(node[AST_TAG_ID]),
                                                        GetPrint(node[AST_TAG_ELIST])));
    }
    virtual void VisitElist(const Object &node)
    {
        if (node[AST_TAG_ELIST])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintElist(
                                                            GetPrint(node[AST_TAG_ELIST]),
                                                            GetPrint(node[AST_TAG_EXPR])));
        }
        else if (node[AST_TAG_EXPR])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintElist(
                                                            "",
                                                            GetPrint(node[AST_TAG_EXPR])));
        }
        else
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintElist("", ""));
        }
    }
    virtual void VisitObjectDef(const Object &node)
    {
        if (node[AST_TAG_ELIST])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintObjectdef(GetPrint(node[AST_TAG_ELIST])));
        }
        else if (node[AST_TAG_INDEXED])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintObjectdef(GetPrint(node[AST_TAG_INDEXED])));
        }
    }
    virtual void VisitIndexed(const Object &node)
    {
        if (node[AST_TAG_INDEXED])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintIndexed(
                                                            GetPrint(node[AST_TAG_INDEXED]),
                                                            GetPrint(node[AST_TAG_INDEXEDELEM])));
        }
        else
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintIndexed("", GetPrint(node[AST_TAG_INDEXEDELEM])));
        }
    }
    virtual void VisitIndexedElem(const Object &node)
    {
        const_cast<Object &>(node).Set(PRINT_VALUE, PrintIndexedElem(
                                                        GetPrint(node[AST_TAG_EXPR_LEFT]),
                                                        GetPrint(node[AST_TAG_EXPR_RIGHT])));
    }
    virtual void VisitBlock(const Object &node)
    {
        const_cast<Object &>(node).Set(PRINT_VALUE, PrintBlock(GetPrint(node[AST_TAG_STMTS])));
    }
    virtual void VisitFuncDef(const Object &node)
    {
        if (node[AST_TAG_ID])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintFuncdef(
                                                            GetPrint(node[AST_TAG_ID]),
                                                            GetPrint(node[AST_TAG_IDLIST]),
                                                            GetPrint(node[AST_TAG_BLOCK])));
        }
        else
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintFuncdef("",
                                                                     GetPrint(node[AST_TAG_IDLIST]),
                                                                     GetPrint(node[AST_TAG_BLOCK])));
        }
    }
    virtual void VisitConst(const Object &node)
    {
        const_cast<Object &>(node).Set(PRINT_VALUE, PrintConst());
    }
    virtual void VisitNumber(const Object &node) {}
    virtual void VisitIdlist(const Object &node)
    {
        if (node[AST_TAG_IDLIST])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintIdlist(
                                                            GetPrint(node[AST_TAG_IDLIST]),
                                                            GetPrint(node[AST_TAG_ID])));
        }
        else if (node[AST_TAG_ID])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintIdlist("", GetPrint(node[AST_TAG_ID])));
        }
        else
        {
            const_cast<Object &>(node).Set(PRINT_VALUE, PrintIdlist("", ""));
        }
    }
    virtual void VisitIf(const Object &node)
    {
        if (node[AST_TAG_ELSE_STMT])
        {
            const_cast<Object &>(node).Set(PRINT_VALUE,
                                           PrintIf(
                                               GetPrint(node[AST_TAG_EXPR]),
                                               GetPrint(node[AST_TAG_IF_STMT]),
                                               GetPrint(node[AST_TAG_ELSE_STMT])));
        }
        else
        {
            const_cast<Object &>(node).Set(PRINT_VALUE,
                                           PrintIf(
                                               GetPrint(node[AST_TAG_EXPR]),
                                               GetPrint(node[AST_TAG_IF_STMT]), ""));
        }
    }
    virtual void VisitWhile(const Object &node)
    {
        const_cast<Object &>(node).Set(PRINT_VALUE, PrintWhile(
                                                        GetPrint(node[AST_TAG_EXPR]),
                                                        GetPrint(node[AST_TAG_STMT])));
    }
    virtual void VisitFor(const Object &node)
    {
        const_cast<Object &>(node).Set(PRINT_VALUE, PrintFor(
                                                        GetPrint(node[AST_TAG_INIT]),
                                                        GetPrint(node[AST_TAG_EXPR]),
                                                        GetPrint(node[AST_TAG_FORCOND]),
                                                        GetPrint(node[AST_TAG_STMT])));
    }
    virtual void VisitReturn(const Object &node)
    {
        const_cast<Object &>(node).Set(PRINT_VALUE, PrintReturn(GetPrint(node[AST_TAG_EXPR])));
    }

    virtual TreeVisitor *Clone(void) const = 0;
    PrintTreeVisitor(void);
    PrintTreeVisitor(const PrintTreeVisitor &) = default;
};