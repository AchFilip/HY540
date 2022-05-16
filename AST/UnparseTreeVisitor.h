#pragma once
#include "./Object.h"
#include "./TreeVisitor.h"
#include "./ValueStack.h"

#define UNPARSE_VALUE "$$val"

class UnparseTreeVisitor final : public TreeVisitor
{
private:
    //-----------------Extra explanation-----------------
    // Some functions have an integer input called mode
    // This was done in order to generilize a function to work for every option (of the given grammatic rule)
    // Every function that has a mode variable there will be more information on which mode is what.
    //---------------------------------------------------
    // Util
    // Delete later
    void PrintTheUnparsedString(std::string str){
        std::cout << "\033[1;31mThe unparsed string is the: > " << str << " <\033[0m" << std::endl;
    }

    const std::string UnparseVarDecl();

    const std::string UnparseStmts(const std::string &stmts, const std::string &stmt)
    {
        std::string str(stmts + " " + stmt + ";");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseStmt(const std::string &input, bool semicon = false)
    {
        std::string str;
        semicon ? str = input + ";" : str = input;
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseExpr(const std::string &term)
    {
        std::string str(term);
        PrintTheUnparsedString(str);
        return str;
    }

    // Same with the above
    const std::string UnparseTerm(const std::string &term, const std::string mode = "")
    {
        std::string str;
        if (mode == "(expr)")
        {
            str = "(" + term + ")";
        }
        else if (mode == "-expr")
        {
            str = "-" + term;
        }
        else if (mode == "notlvalue")
        {
            str = "!" + term;
        }
        else if (mode == "++lvalue")
        {
            str = "++" + term;
        }
        else if (mode == "lvalue++")
        {
            str = term + "++";
        }
        else if (mode == "--lvalue")
        {
            str = "--" + term;
        }
        else if (mode == "lvalue--")
        {
            str = term + "--";
        }
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseArithmeticExpr(const std::string &expr1, const std::string &op, const std::string &expr2)
    {
        std::string str(expr1 + " " + op + " " + expr2);
        PrintTheUnparsedString(str);
        return str;
    }
    // Same with the above
    const std::string UnparseRelationalExpr(const std::string &expr1, const std::string &op, const std::string &expr2)
    {
        std::string str(expr1 + " " + op + " " + expr2);
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseAssignExpr(const std::string &lvalue, const std::string &expr)
    {
        std::string str(lvalue + " = " + expr + ";");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparsePrimary(int mode, const std::string &input)
    {
        std::string str;
        switch (mode)
        {
        case 0: // lvalue
            str = input;
            break;
        case 1: // call
            str = input;
            break;
        case 2: // objectdef
            str = input;
            break;
        case 3: // ( funcdef )
            str = "( " + input + " )";
            break;
        case 4: // const
            str = input;
            break;
        default:
            std::cout << "Wrong Mode.. Exiting" << std::endl;
            exit(0);
        }
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseLvalue(const std::string &input, const std::string &indicator = "")
    {
        std::string str;
        if (indicator.empty())
            str = input;
        else if (indicator == "local_id")
            str = "local " + str;
        else if (indicator == "doubledots_id")
            str = ":: " + str;
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseMember(int mode, const std::string &input1, const std::string &input2)
    {
        std::string str = "";
        switch (mode)
        {
        case 0: // lvalue . id
            str = input1 + " . " + input2;
            break;
        case 1: // lvalue [ expr ]
            str = input1 + " [ " + input2 + " ] ";
            break;
        case 2: // call . id
            str = input1 + " . " + input2;
            break;
        case 3: // call [ expr ]
            str = input1 + " [ " + input2 + " ] ";
            break;
        default:
            break;
        }
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseCall(int mode, const std::string &input1, const std::string &input2)
    {
        std::string str = "";
        switch (mode)
        {
        case 0: // call '(' elist ')'
            str = input1 + " ( " + input2 + " ) ";
            break;
        case 1: // lvalue callsuffix
            str = input1 + " " + input2;
            break;
        case 2: //'(' funcdef ')' '(' elist ')'
            str = "( " + input1 + " ) " + "( " + input2 + " ) ";
            break;
        default:
            break;
        }
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseNormCall(const std::string &elist)
    {
        std::string str("( " + elist + " )");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseMethodCall(const std::string &id, const std::string &elist)
    {
        std::string str("::" + id + " ( " + elist + " )");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseElist(const std::string &expr, const std::string &elist = "")
    {
        std::string str;
        elist.empty() ? str = expr : str = elist + " , " + expr;
        PrintTheUnparsedString(str);
        return str;
    }

    // Named the incoming var ptr because if the parser gives
    //  either elist or indexed, the print is the same.
    const std::string UnparseObjectDef(const std::string &ptr)
    {
        std::string str("[ " + ptr + " ]");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseIndexed(const std::string &indexedelem, const std::string &indexed = "")
    {
        std::string str;
        indexed.empty() ? str = indexedelem : str = indexed + " , " + indexedelem;
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseIndexedElem(const std::string &expr1, const std::string &expr2)
    {
        std::string str("{ " + expr1 + " : " + expr2 + " }");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseBlock(const std::string &stmts)
    {
        std::string str("{ " + stmts + " }");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseFuncDef(const std::string& idlist,const std::string& block,const std::string& id = ""){
        std::string str;
        id.empty() ? str = "function ( " + idlist + " ) " + block : "function " + id + " ( " + idlist + " ) " + block;
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseConst(const std::string &input)
    {
        std::string str(input);
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseNumber(const std::string &num)
    {
        std::string str;
        str = num;
        PrintTheUnparsedString(str);
        return num;
    }

    const std::string UnparseIdlist(const std::string &id, const std::string &idlist = "")
    {
        std::string str;
        idlist.empty() ? str = id : str = idlist + " , " + id;
        PrintTheUnparsedString(str);
        return str;
    }
    const std::string UnparseWhile(const std::string &expr, const std::string &stmt)
    {
        std::string str("while ( " + expr + " ) " + stmt);
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseForStmt(const std::string &elist1, const std::string &expr, const std::string elist2, const std::string stmt)
    {
        std::string str("for ( " + elist1 + " ; " + expr + " ; " + elist2 + " ) " + stmt + ";");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseReturn(const std::string& expr = ""){
        std::string str("return " + expr + ";");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseIf(const std::string &expr, const std::string &stmt)
    {
        std::string str(("if (" + expr + ") " + stmt + ";"));
        PrintTheUnparsedString(str);
        return str;
    }

    std::string GetUnparsed(const Value *val) const
    {
        return val->ToObject_NoConst()->GetAndRemove(UNPARSE_VALUE).ToString();
    }

public:
    virtual void VisitVarDecl(const Object &node) override {}

    virtual void VisitStmts(const Object &node) override
    {
        const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseStmt(GetUnparsed(node[AST_TAG_EXPR])));
        // const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseStmt(GetPrint(node[AST_TAG_EXPR])));
    }

    virtual void VisitStmt(const Object &node) override
    {
        if (node[AST_TAG_EXPR] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseStmt(GetUnparsed(node[AST_TAG_EXPR]), true));
        }
        else if (node[AST_TAG_IF] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseStmt(GetUnparsed(node[AST_TAG_IF]), false));
        }
        else if (node[AST_TAG_WHILE] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseStmt(GetUnparsed(node[AST_TAG_WHILE]), false));
        }
        else if (node[AST_TAG_FOR] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseStmt(GetUnparsed(node[AST_TAG_FOR]), false));
        }
        else if (node[AST_TAG_RETURNSTMT] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseStmt(GetUnparsed(node[AST_TAG_RETURNSTMT]), false));
        }
        else if (node[AST_TAG_BREAK] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseStmt(GetUnparsed(node[AST_TAG_BREAK]), false));
        }
        else if (node[AST_TAG_CONTINUE] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseStmt(GetUnparsed(node[AST_TAG_CONTINUE]), false));
        }
        else if (node[AST_TAG_BLOCK] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseStmt(GetUnparsed(node[AST_TAG_BLOCK]), false));
        }
        else if (node[AST_TAG_FUNCDEF] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseStmt(GetUnparsed(node[AST_TAG_FUNCDEF]), false));
        }
    }

    virtual void VisitExpr(const Object &node) override
    {
        if (node[AST_TAG_ASSIGNEXPR] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseExpr(GetUnparsed(node[AST_TAG_ASSIGNEXPR])));
        }
        else if (node[AST_TAG_EXPR_LEFT] != nullptr && node[AST_TAG_EXPR_RIGHT] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseArithmeticExpr(GetUnparsed(node[AST_TAG_EXPR_LEFT]), GetUnparsed(node[AST_TAG_DISAMBIGUATE_OBJECT]), GetUnparsed(node[AST_TAG_EXPR_RIGHT])));
        }
        else if (node[AST_TAG_TERM])
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseExpr(GetUnparsed(node[AST_TAG_TERM])));
        }
    }

    virtual void VisitArithmeticExpr(const Object &node) override {}
    virtual void VisitRelationalExpr(const Object &node) override {}

    virtual void VisitAssignexpr(const Object &node) override
    {
        const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseAssignExpr(GetUnparsed(node[AST_TAG_LVALUE]), GetUnparsed(node[AST_TAG_EXPR])));
    }

    virtual void VisitTerm(const Object &node) override
    {
        if (node[AST_TAG_PRIMARY] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseTerm(GetUnparsed(node[AST_TAG_PRIMARY]), ""));
        }
        else if (node[AST_TAG_EXPR] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseTerm(GetUnparsed(node[AST_TAG_EXPR]), node[AST_TAG_OBJECTDEF]->ToString()));
        }
        else if (node[AST_TAG_LVALUE] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseTerm(GetUnparsed(node[AST_TAG_LVALUE]), node[AST_TAG_OBJECTDEF]->ToString()));
        }
    }

    virtual void VisitPrimary(const Object &node) override
    {
        if (node[AST_TAG_LVALUE] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparsePrimary(0, GetUnparsed(node[AST_TAG_LVALUE])));
        }
        else if (node[AST_TAG_CALL] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparsePrimary(1, GetUnparsed(node[AST_TAG_CALL])));
        }
        else if (node[AST_TAG_OBJECTDEF] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparsePrimary(2, GetUnparsed(node[AST_TAG_OBJECTDEF])));
        }
        else if (node[AST_TAG_FUNCDEF] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparsePrimary(3, GetUnparsed(node[AST_TAG_FUNCDEF])));
        }
        else if (node[AST_TAG_CONST] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparsePrimary(4, GetUnparsed(node[AST_TAG_CONST])));
        }
    }

    virtual void VisitLvalue(const Object &node) override
    {
        if (node[AST_TAG_ID] != nullptr)
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseLvalue(GetUnparsed(node[AST_TAG_ID]), node[AST_TAG_DISAMBIGUATE_OBJECT]->ToString()));
        }
        else if (node[AST_TAG_MEMBER])
        {
            const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseLvalue(GetUnparsed(node[AST_TAG_MEMBER])));
        }
    }

    virtual void VisitMember(const Object &node) override
    {
        if (node[AST_TAG_LVALUE] != nullptr)
        {
            if (node[AST_TAG_ID] != nullptr)
            {
                const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseMember(0, GetUnparsed(node[AST_TAG_LVALUE]), GetUnparsed(node[AST_TAG_ID])));
            }
            else if (node[AST_TAG_EXPR] != nullptr)
            {
                const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseMember(1, GetUnparsed(node[AST_TAG_LVALUE]), GetUnparsed(node[AST_TAG_EXPR])));
            }
        }
        else if (node[AST_TAG_CALL] != nullptr)
        {
            if (node[AST_TAG_ID] != nullptr)
            {
                const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseMember(2, GetUnparsed(node[AST_TAG_CALL]), GetUnparsed(node[AST_TAG_ID])));
            }
            else if (node[AST_TAG_EXPR] != nullptr)
            {
                const_cast<Object &>(node).Set(UNPARSE_VALUE, UnparseMember(3, GetUnparsed(node[AST_TAG_CALL]), GetUnparsed(node[AST_TAG_EXPR])));
            }
        }
    }
    
    virtual void VisitCall(const Object &node) override{ //not tested
        if(node.children.count(AST_TAG_CALL))
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseCall(
                    0,
                    GetUnparsed(node[AST_TAG_CALL]),
                    GetUnparsed(node[AST_TAG_ELIST])
                )
            );
        else if(node.children.count(AST_TAG_CALLSUFFIX))
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseCall(
                    1,
                    GetUnparsed(node[AST_TAG_LVALUE]),
                    GetUnparsed(node[AST_TAG_CALLSUFFIX])
                )
            );
        else if(node.children.count(AST_TAG_FUNCDEF))
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseCall(
                    0,
                    GetUnparsed(node[AST_TAG_FUNCDEF]),
                    GetUnparsed(node[AST_TAG_ELIST])
                )
            );
    } 
    
    virtual void VisitNormCall(const Object &node) override{ //not tested
        const_cast<Object&>(node).Set(
            UNPARSE_VALUE,
            UnparseNormCall(
                GetUnparsed(node[AST_TAG_ELIST])
            )
        );    
    }

    virtual void VisitMethodCall(const Object &node) override{ //not tested
        const_cast<Object&>(node).Set(
            UNPARSE_VALUE,
            UnparseMethodCall(
                GetUnparsed(node[AST_TAG_ID]),
                GetUnparsed(node[AST_TAG_ELIST])
            )
        );
    } 

    virtual void VisitElist(const Object &node) override{ //not tested
        if(false) return; //check gia nill


        if(node.children.count(AST_TAG_ELIST))
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseElist(
                    GetUnparsed(node[AST_TAG_EXPR]),
                    GetUnparsed(node[AST_TAG_ELIST])
                )
            );
        else
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseElist(
                    GetUnparsed(node[AST_TAG_EXPR])
                )
            );
    } 

    virtual void VisitObjectDef(const Object &node) override{ //not tested
        if(node.children.count(AST_TAG_ELIST))
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseObjectDef(
                    GetUnparsed(node[AST_TAG_ELIST])
                )
            );
        else if(node.children.count(AST_TAG_INDEXED))
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseObjectDef(
                    GetUnparsed(node[AST_TAG_INDEXED])
                )
            );
    } 
    
    virtual void VisitIndexed(const Object &node) override{
        if(node.children.count(AST_TAG_INDEXED)) //not tested
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseIndexed(
                    GetUnparsed(node[AST_TAG_INDEXEDELEM]),
                    GetUnparsed(node[AST_TAG_INDEXED])
                )
            );
        else
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseIndexed(
                    GetUnparsed(node[AST_TAG_INDEXEDELEM])
                )
            );
    }

    virtual void VisitIndexedElem(const Object &node) override{ //not tested
        const_cast<Object&>(node).Set(
            UNPARSE_VALUE,
            UnparseIndexedElem(
                GetUnparsed(node[AST_TAG_EXPR_LEFT]),
                GetUnparsed(node[AST_TAG_EXPR_RIGHT])
            )
        );
    } 

    virtual void VisitBlock(const Object &node) override{
        const_cast<Object&>(node).Set(
            UNPARSE_VALUE,
            UnparseBlock(
                GetUnparsed(node[AST_TAG_STMTS])
            )
        ); 
    }

    virtual void VisitFuncDef(const Object &node) override{ //not tested
        if(node.children.count(AST_TAG_ID))
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseFuncDef(
                    GetUnparsed(node[AST_TAG_IDLIST]),
                    GetUnparsed(node[AST_TAG_BLOCK]),
                    GetUnparsed(node[AST_TAG_ID])
                )
            );
        else
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseFuncDef(
                    GetUnparsed(node[AST_TAG_IDLIST]),
                    GetUnparsed(node[AST_TAG_BLOCK])
                )
            );
    } 

    virtual void VisitConst(const Object &node) override{ 
        //In parser we create value and not object.
    }

    virtual void VisitNumber(const Object &node) override{} //?
    
    virtual void VisitIdlist(const Object &node) override{
        
        if(false) return; //Check for nil
        
        if(node.children.count(AST_TAG_ID))
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseIdlist(
                    GetUnparsed(node[AST_TAG_IDLIST]),
                    GetUnparsed(node[AST_TAG_ID])
                )
            );
        else
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseIdlist(
                    GetUnparsed(node[AST_TAG_IDLIST])
                )
            );
    }

    virtual void VisitWhile(const Object &node) override{ //den bike kan
        const_cast<Object&>(node).Set(
            UNPARSE_VALUE,
            UnparseWhile(
                GetUnparsed(node[AST_TAG_EXPR]),
                GetUnparsed(node[AST_TAG_STMT])
            )
        );
    }

    virtual void VisitFor(const Object &node) override{ //crashed
        const_cast<Object&>(node).Set(
            UNPARSE_VALUE,
            UnparseForStmt(
                GetUnparsed(node[AST_TAG_INIT]),
                GetUnparsed(node[AST_TAG_EXPR]),
                GetUnparsed(node[AST_TAG_FORCOND]),
                GetUnparsed(node[AST_TAG_STMT])
            )
        );
    }
    
    virtual void VisitReturn(const Object &node)
    {
        if(false) //check if node is nil..?
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseReturn()
            );
        else
            const_cast<Object&>(node).Set(
                UNPARSE_VALUE,
                UnparseReturn(
                    GetUnparsed(node[AST_TAG_EXPR])
                )
            ); 
    }
    
    virtual void VisitIf(const Object& node) override{ 
        const_cast<Object&>(node).Set(
            UNPARSE_VALUE,
            UnparseIf(
                GetUnparsed(node[AST_TAG_IF]), //AST_TAG_IF_COND
                GetUnparsed(node[AST_TAG_STMT])
            )
        );
    }

    virtual TreeVisitor *Clone(void) const {};
    UnparseTreeVisitor(void) = default;
    UnparseTreeVisitor(const UnparseTreeVisitor &) = default;
};