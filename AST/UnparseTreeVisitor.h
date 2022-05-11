#define UNPARSE_VALUE "$$val"

#include "./TreeVisitor.h"
#include "./ValueStack.h"
class UnparseTreeVisitor final : public TreeVisitor {
private:

    //Util
    //Delete later
    void PrintTheUnparsedString(std::string str){
        std::cout << "The unparsed string is the:" << std::endl << str << std::endl;
    }

    const std::string UnparseVarDecl();

    const std::string UnparseStmts(const std::string& stmts,const std::string& stmt){
        std::string str(stmts + " " + stmt + ";");
        PrintTheUnparsedString(str);
        return str;
    }
    const std::string UnparseStmt();
    const std::string UnparseExpr();
    const std::string UnparseArithmeticExpr();
    const std::string UnparseRelationalExpr();

    const std::string UnparseAssignExpr(const std::string& lvalue,const std::string& expr){
        std::string str(lvalue + " = " + expr + ";");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseTerm();
    const std::string UnparsePrimary();
    const std::string UnparseLvalue();
    const std::string UnparseMember();
    const std::string UnparseCall();
    const std::string UnparseNormCall();
    const std::string UnparseMethodCall();
    
    const std::string UnparseElist(const std::string& expr, const std::string& elist = ""){
        std::string str;
        elist.empty() ? str = expr : str = elist + " , " + expr;
        PrintTheUnparsedString(str);
        return str;
    }

    //Named the incoming var ptr because if the parser gives
    // either elist or indexed, the print is the same.
    const std::string UnparseObjectDef(const std::string& ptr){
        std::string str("[ " + ptr + " ]");
        PrintTheUnparsedString(str);
        return str;
    }
    const std::string UnparseIndexed();

    const std::string UnparseIndexedElem(const std::string& expr1, const std::string& expr2){
        std::string str("{ " + expr1 + " : " + expr2 + " }");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseBlock(const std::string& stmts){
        std::string str("{ " + stmts + " }");
        PrintTheUnparsedString(str);
        return str;
    }
    const std::string UnparseFuncDef();
    const std::string UnparseConst();
    const std::string UnparseNumber();

    const std::string UnparseIdlist(const std::string& id, const std::string& idlist = ""){
        std::string str;
        idlist.empty() ? str = id : str = idlist + " , " + id;
        PrintTheUnparsedString(str);
        return str;
    }
    const std::string UnparseWhile(const std::string& expr, const std::string& stmt){
        std::string str("while ( " + expr + " ) " + stmt);
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseForStmt(const std::string& elist1, const std::string& expr, const std::string elist2, const std::string stmt){
        std::string str("for ( " + elist1 + " ; " + expr + " ; " + elist2 + " ) " + stmt + ";");
        PrintTheUnparsedString(str);
        return str;
    }

    const std::string UnparseReturn(const std::string& expr = ""){
        std::string str("return " + expr + ";");
        PrintTheUnparsedString(str);
        return str;
    }
    const std::string UnparseIf(const std::string& expr, const std::string& stmt){
        std::string str(("if (" + expr + ") " + stmt + ";"));
        PrintTheUnparsedString(str);
        return str;
    }

    std::string GetUnparsed(const Value* val) const{
        return val->ToObject_NoConst()->GetAndRemove(UNPARSE_VALUE).ToString();
    }
public:

    virtual void VisitVarDecl(const Object &node) override{} //?
    virtual void VisitStmts(const Object &node) override{}
    virtual void VisitStmt(const Object &node) override{} //?
    virtual void VisitExpr(const Object &node) override{} //?
    virtual void VisitArithmeticExpr(const Object &node) override{} //?
    virtual void VisitRelationalExpr(const Object &node) override{} //?
    virtual void VisitAssignxpr(const Object &node) override{}  
    virtual void VisitTerm(const Object &node) override{} //?
    virtual void VisitPrimary(const Object &node) override{} //?
    virtual void VisitLvalue(const Object &node) override{} //?
    virtual void VisitMember(const Object &node) override{} //?
    virtual void VisitCall(const Object &node) override{} //?
    virtual void VisitNormCall(const Object &node) override{} //?
    virtual void VisitMethodCall(const Object &node) override{} //?
    virtual void VisitElist(const Object &node) override{} 
    virtual void VisitObjectDef(const Object &node) override{} 
    virtual void VisitIndexed(const Object &node) override{} //?
    virtual void VisitIndexedElem(const Object &node) override{} 
    virtual void VisitBlock(const Object &node) override{} 
    virtual void VisitFuncDef(const Object &node) override{} //?
    virtual void VisitConst(const Object &node) override{} //?
    virtual void VisitNumber(const Object &node) override{} //?
    virtual void VisitIdlist(const Object &node) override{}
    virtual void VisitWhile(const Object &node) override{} 
    virtual void VisitFor(const Object &node) override{}
    virtual void VisitReturn(const Object &node) override{}
    virtual void VisitIf(Object& node) override{ //maybe add else as well or do other visit.
        GetUnparsed(node[AST_TAG_EXPR]);
        std::cout << "Hereeee: " << "11" << std::endl;
        std::cout << "Finished..";
    }

    int test123;
};