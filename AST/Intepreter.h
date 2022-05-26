#pragma once
#include <functional>
#include "./TreeTags.h"
#include "./Object.h"
#include "./Value.h"
#include "./ValueStack.h"
#include "./EvalDispatcher.h"
#include "./DebugAST.h"
#include <iostream>
#define PRINT_BLUE_LINE(to_print)   std::cout << "\033[1;36m" << to_print << "\033[0m\n"
#define PRINT_YELLOW_LINE(to_print) std::cout << "\033[1;33m" << to_print << "\033[0m\n"
#define NORMAL_PRINT_LINE(to_print) std::cout <<  to_print << "\n"

const Value* GetArgument(Object& env, unsigned argNo, const std::string& optArgName){
        auto* arg = env[optArgName];
        if(!arg)
            arg = env[argNo];
        return arg;
}

std::string ObjectToString(Object* obj, std::string toPrint, std::string startingTab){      
    if(obj->children.begin() == obj->children.end())
        return startingTab + "Object { }";   
    
    toPrint += startingTab + "Object {\n" ;
    startingTab += "\t";

    for(auto it = obj->children.begin(); it != obj->children.end(); ){  
        if(it->second.GetType() != Value::ObjectType){
            std::string id = it->first;
            if(id.size() > 2 && id.substr(2,id.size()) == "000000")
                id = id.substr(0,1);
            toPrint += startingTab + id + ": " + it->second.Stringify() + "\n";
        }
            
        else if(it->second.GetType() == Value::ObjectType){
            std::string objString = ObjectToString((it->second).ToObject_NoConst(), "", startingTab);
            objString = objString.substr(startingTab.size(), objString.size());

            std::string id = it->first;
            if(id.substr(2,id.size()) == "000000")
                id = id.substr(0,1);

            toPrint += startingTab + id + ": " + objString + "\n";
        }            
        it++;                         
    }
    toPrint += startingTab.substr(0,startingTab.size()-1) + "}";
    return toPrint;
}

class Interpreter
{
private:
    EvalDispatcher *dispatcher;
    ValueStack *envStack;
    DebugAST debug;

    struct BreakException{};
    struct ContinueException{};
    struct ReturnException{};

    const Value Eval(Object &node)
    {
        debug.ObjectPrintChildren(node, node[AST_TAG_LINE_KEY]->Stringify(), node[AST_TAG_TYPE_KEY]->Stringify());
        return dispatcher->Eval(node);
    }
    const Value EvalStmts(Object &node)
    {
        if (node[AST_TAG_STMTS]->GetType() != Value::NilType && node[AST_TAG_STMT]->GetType() != Value::NilType)
        {
            Eval(*node[AST_TAG_STMTS]->ToObject_NoConst());
            Eval(*node[AST_TAG_STMT]->ToObject_NoConst());
            return _NIL_;
        }
        else if (node[AST_TAG_STMTS]->GetType() != Value::NilType)
        {
            Eval(*node[AST_TAG_STMTS]->ToObject_NoConst());
        }
        else if (node[AST_TAG_STMT]->GetType() != Value::NilType)
        {
            Eval(*node[AST_TAG_STMT]->ToObject_NoConst());
        }
        else
        {
            return _NIL_;
        }
        return _NIL_;
    }
    const Value EvalStmt(Object &node)
    {
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

        if(node[stmtType]->GetType() == Value::ObjectType) // Retval can be nil
            return Eval(*node[stmtType]->ToObject_NoConst());
        else
            return *node[stmtType];
    }
    const Value EvalExpr(Object &node)
    {
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
        assert(false);
    }
    const Value EvalTerm(Object &node)
    {
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
            if (operatorStr == "notlvalue")
            {
                Value lvalue = Eval(*node[AST_TAG_LVALUE]->ToObject_NoConst());
                return !lvalue;
            }
            else if (operatorStr == "lvalue++")
            {
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
            else if (operatorStr == "lvalue--")
            {
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
        Value &lvalue = const_cast<Value &>(EvalLvalue(*node[AST_TAG_LVALUE]->ToObject_NoConst(), true));
        Value expr = Eval(*node[AST_TAG_EXPR]->ToObject_NoConst()); // Rvalue eval
        
        if(lvalue.GetType() == Value::ProgramFunctionType){
            if(expr.GetType() == Value::ObjectType){
                lvalue.SetClosure(expr.ToObject_NoConst());
                // lvalue = *(new Value(*lvalue.ToProgramFunctionAST_NoConst(), expr.ToObject_NoConst()));
                debug.ObjectPrintChildren(*(lvalue.ToProgramFunctionClosure_NoConst()));
            }
            else
                assert(false && "function.$closure == expr. Expr can only be an object");
        }
        else
            lvalue = expr;

        return lvalue;
    }
    const Value EvalPrimary(Object &node)
    {

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
        else if (node[AST_TAG_CALL])
        {
            return Eval(*node[AST_TAG_CALL]->ToObject_NoConst());
        }
        else if (node[AST_TAG_FUNCDEF]){
            return Eval(*node[AST_TAG_FUNCDEF]->ToObject_NoConst());
        }
        else
            assert(false && "Not implmeneted yet for other types");
    }
    const Value EvalRvalue(Object &node)
    {

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
                    debug.LookupPrint("Lookup of \"" + id + "\" Rvalue: found " + lookup->Stringify());
                    return *lookup;
                }
                else
                {
                    debug.LookupPrint("Lookup of \"" + id + "\" Rvalue: not_found ");
                    assert(false && "Lookup didn't find anything");
                    return Value();
                }
            }
            else if (disambiguate == "local id")
            {
                // Can only be used as lvalue
                return EvalLvalue(node, true);
            }
            else if (disambiguate == "doubledots id")
            {
                Object *curr_scope = &GetCurrentScope();
                while ((*curr_scope)[OUTER_SCOPE_KEY])
                {
                    curr_scope = (*curr_scope)[OUTER_SCOPE_KEY]->ToObject_NoConst();
                }
                const Value *lookup = ScopeLookup(*curr_scope, id);
                if (lookup)
                {
                    debug.LookupPrint("Lookup Rvalue: found " + lookup->Stringify());
                    return *lookup;
                }
                else
                {
                    assert(false && "Lookup didn't find anything");
                    return Value();
                }
            }
        }
        else if (node[AST_TAG_MEMBER])
        {
            return Eval(*node[AST_TAG_MEMBER]->ToObject_NoConst());
        }
        else
            assert(false && "Not implemented yet");
        assert(false);
    }
    const Value EvalMember(Object &node)
    {
        std::string disambiguate = node[AST_TAG_DISAMBIGUATE_OBJECT]->Stringify();
        if (node[AST_TAG_LVALUE])
        {
            return EvalObjGet(node);
        }
        else if (node[AST_TAG_CALL])
        {
            const Value call = Eval(*node[AST_TAG_CALL]->ToObject_NoConst());
            if (disambiguate == ".id")
            {
            }
            else if (disambiguate == "[expr]")
            {
            }
            else
                assert(false && "Impossible");
        }
        return _NIL_;
    }
    const Value EvalCall(Object &node)
    {

        try{
            if(node[AST_TAG_LVALUE]){

                Value functionValue = Eval(*node[AST_TAG_LVALUE]->ToObject_NoConst());
                Value args = Eval(*node[AST_TAG_CALLSUFFIX]->ToObject_NoConst());

                if(functionValue.GetType() == Value::ProgramFunctionType){
                    //Adjust Scopes
                    PushScopeSpace(functionValue.ToProgramFunctionClosure_NoConst());
                    //Push arguments
                    Object& ast = *functionValue.ToProgramFunctionAST_NoConst();  
                    //Make call
                    EvalFunctionBlock(ast, args);
                }
                else if(functionValue.GetType() == Value::LibraryFunctionType){
                    //Adjust Scopes
                    PushScopeSpace();
                    PushNested();
                    //Push Arguments
                    if(args.GetType() != Value::NilType)
                        PushLibraryFunctionArgumentsToScope(*args.ToObject_NoConst());
                    //MakeCall
                    functionValue.ToLibraryFunction()(GetCurrentScope());
                }
            }
            else if(node[AST_TAG_FUNCDEF] || node[AST_TAG_CALL]){ // Anonymous function
                Value functionValue = Eval(node[AST_TAG_CALL] ? *node[AST_TAG_CALL]->ToObject_NoConst() : *node[AST_TAG_FUNCDEF]->ToObject_NoConst());
                Value args = Eval(*node[AST_TAG_ELIST]->ToObject_NoConst());

                //Adjust Scopes
                PushScopeSpace(functionValue.ToProgramFunctionClosure_NoConst());
                PushNested();
                //Push arguments
                Object& ast = *functionValue.ToProgramFunctionAST_NoConst();  
                if(ast[AST_TAG_ARGUMENT_LIST]->GetType() != Value::NilType && args.GetType() != Value::NilType)
                    PushProgramFunctionArgumentsToScope(*args.ToObject_NoConst(), *ast[AST_TAG_ARGUMENT_LIST]->ToObject_NoConst());                
                //Make call
                Eval(ast);
            }
        }
        catch(const ReturnException &){}
        // Get retval from ret register
        debug.ObjectPrintChildren(GetCurrentScope());
        Value retval = RETVAL_GET();

        //Func Exit
        PopScope();
        PopScopeSpace();

        return retval;
    }
    void PushProgramFunctionArgumentsToScope(Object& elist, Object& idlist){
        auto& scope = GetCurrentScope();
        for(auto elist_it = elist.children.begin(), idlist_it = idlist.children.begin(); 
            elist_it != elist.children.end() && idlist_it != idlist.children.end(); 
            ++elist_it, ++idlist_it
        )
            scope.Set((idlist_it->second).Stringify(), elist_it->second);
        return;
    }
    void PushLibraryFunctionArgumentsToScope(Object& elist){
        auto& scope = GetCurrentScope();
        int i = 0;
        for(auto elist_it = elist.children.begin(); 
            elist_it != elist.children.end(); 
            ++elist_it, i++
        )
            scope.Set(i, elist_it->second);
    }
    const Value EvalCallSuffix(Object &node)
    {
        if(node[AST_TAG_NORMCALL]){
            if(node[AST_TAG_NORMCALL]->GetType() != Value::NilType){
                return Eval(*node[AST_TAG_NORMCALL]->ToObject_NoConst());
            }        
            else
                return _NIL_;
        }
        else if (node[AST_TAG_METHODCALL])
            assert(false && "Methodcall not implemented yet");
        assert(false);
    }
    const Value EvalNormCall(Object &node)
    {
        if (node[AST_TAG_ELIST]->GetType() != Value::NilType){
            return Eval(*node[AST_TAG_ELIST]->ToObject_NoConst());
        }
        else
            return _NIL_;
    }
    const Value EvalMethodCall(Object &node)
    {
        return _NIL_;
    }
    const Value EvalTreeElist(Object &node, Object &evaluatedElist, int &counter)
    {
        if (node[AST_TAG_ELIST])
        {
            Value expr;
            if(node[AST_TAG_EXPR]->GetType() == Value::ObjectType)
                expr = Eval(*node[AST_TAG_EXPR]->ToObject_NoConst());
            else
                expr = *node[AST_TAG_EXPR];
            evaluatedElist.Set(counter, expr);

            Value elist;
            if(node[AST_TAG_ELIST]->GetType() == Value::ObjectType)
                elist = EvalTreeElist(*node[AST_TAG_ELIST]->ToObject_NoConst(), evaluatedElist, ++counter);
            else{
                elist = *node[AST_TAG_ELIST];
            }
            evaluatedElist.Set(counter, elist);
            return node;
        }
        else if (node[AST_TAG_EXPR])
        {
            if(node[AST_TAG_EXPR]->GetType() == Value::ObjectType)
                return Eval(*node[AST_TAG_EXPR]->ToObject_NoConst());
            else
                return *node[AST_TAG_EXPR];
        }
        else
        {
            // empty elist
            return _NIL_;
        }
    }
    const Value EvalElist(Object &node)
    {
        Object *elist = new Object();
        if (node[AST_TAG_ELIST] == nullptr && node[AST_TAG_EXPR] != nullptr)
        {
            elist->Set(0, Eval(*node[AST_TAG_EXPR]->ToObject_NoConst()));
            return Value(*elist);
        }
        else if(node[AST_TAG_ELIST] != nullptr)
        {
            int counter = 0; // Counter is passed with reference, so it has to be a var
            EvalTreeElist(node, *elist, counter); 

            // Reverse the order of the elist elements
            ValueStack stack;
            for(int i = 0; elist->children.size(); i++)
            {
                stack.Push(elist->GetAndRemove(i));
            }
            for(int i = 0 ; stack.IsEmpty() == false; i++){
                elist->Set(i, stack.Top());
                stack.Pop();
            }

            return Value(*elist);
        }
        else
            return _NIL_;
    }
    const Value EvalObjectDef(Object &node)
    {
        // TODO: use returned values of eval to create the object;
        if (node[AST_TAG_ELIST])
        {
            if ((node[AST_TAG_ELIST]->GetType() != Value::NilType))
            {
                return Eval(*(node[AST_TAG_ELIST]->ToObject_NoConst()));
            }
            else
                return *(new Value(*(new Object())));
            assert(false);
        }
        else if (node[AST_TAG_INDEXED])
        {
            if ((node[AST_TAG_INDEXED]->GetType() != Value::NilType))
            {
                const Value indexed = Eval(*(node[AST_TAG_INDEXED]->ToObject_NoConst()));
                std::cout << "mark" << std::endl;
                return indexed;
            }
            else
                return *(new Value(*(new Object())));
            assert(false);
        }
        else
            assert(false && "Invalid ObjectDef node");
    }
    const Value EvalTreeIndexed(Object &node, Object &evaluatedIndexed){
        if (node[AST_TAG_INDEXED])
        {
            Value indexedelem = EvalIndexedElem(*node[AST_TAG_INDEXEDELEM]->ToObject_NoConst());
            Object &indexedelemObj = *indexedelem.ToObject_NoConst();
            evaluatedIndexed.Set(indexedelemObj[AST_TAG_EXPR_LEFT]->Stringify(), *indexedelemObj[AST_TAG_EXPR_RIGHT]);

            Value indexed = EvalTreeIndexed(*node[AST_TAG_INDEXED]->ToObject_NoConst(), evaluatedIndexed);
            Object &indexedObj = *indexed.ToObject_NoConst();
            if(indexedObj[AST_TAG_EXPR_LEFT])
                evaluatedIndexed.Set(indexedObj[AST_TAG_EXPR_LEFT]->Stringify(), *indexedObj[AST_TAG_EXPR_RIGHT]);

            return evaluatedIndexed;
        }
        else if (node[AST_TAG_INDEXEDELEM])
        {
            return Eval(*node[AST_TAG_INDEXEDELEM]->ToObject_NoConst());
        }
    }
    const Value EvalIndexedElem(Object &node)
    {
        Object *evaluatedElem = new Object();

        Value l_expr = EvalExpr(*node[AST_TAG_EXPR_LEFT]->ToObject_NoConst());
        evaluatedElem->Set(AST_TAG_EXPR_LEFT, l_expr);

        Value r_expr = EvalExpr(*node[AST_TAG_EXPR_RIGHT]->ToObject_NoConst());
        evaluatedElem->Set(AST_TAG_EXPR_RIGHT, r_expr);

        return *evaluatedElem;
    }
    const Value EvalIndexed(Object &node)
    {
        Object *indexed;
        if(node[AST_TAG_INDEXED]){
            indexed = new Object();
            std::cout << "before eval tree elist" << std::endl;
            EvalTreeIndexed(node, *indexed);
            std::cout << "after eval tree elist" << std::endl;
        }
        else if(node[AST_TAG_INDEXEDELEM]){ 
            indexed = EvalIndexedElem(*node[AST_TAG_INDEXEDELEM]->ToObject_NoConst()).ToObject_NoConst();
            std::cout << "mark" << std::endl;
        }
        else 
            assert(false && "Impossible to have other types in EvalIndexed");

        indexed->Debug_PrintChildren();
        std::cout << "end of eval indexed" << std::endl;
        return *indexed;
    }
    const Value EvalBlock(Object &node)
    {
        if (node[AST_TAG_STMTS]->GetType() != Value::NilType)
        {
            // Push new nested scope
            PushNested();
            // Eval code inside of block
            const Value val = Eval(*node[AST_TAG_STMTS]->ToObject_NoConst());
            // Make sure that scopes have been restored
            auto &currScope = GetCurrentScope();
            bool shouldSliceOuter = currScope[PREVIOUS_SCOPE_KEY] != nullptr;
            if (shouldSliceOuter)
                while (true) // Cringe fast code
                {
                    auto &scope = GetCurrentScope();
                    if(scope[PREVIOUS_SCOPE_KEY] == nullptr)
                        break;
                    else
                        PopScope();
                }
            assert(GetCurrentScope()[OUTER_SCOPE_KEY] != nullptr);
            PopScope();
            if (shouldSliceOuter)
            {
                PushSlice();
            }
            return val;
        }
        else
            return _NIL_;
    }
    const Value EvalFunctionBlock(Object &node, Value &args)
    {
        if (node[AST_TAG_STMTS]->GetType() != Value::NilType)
        {
            // Push new nested scope
            PushNested();
            if(node[AST_TAG_ARGUMENT_LIST]->GetType() != Value::NilType && args.GetType() != Value::NilType)
                    PushProgramFunctionArgumentsToScope(*args.ToObject_NoConst(), *node[AST_TAG_ARGUMENT_LIST]->ToObject_NoConst());                
            // Eval code inside of block
            const Value val = Eval(*node[AST_TAG_STMTS]->ToObject_NoConst());
            // Make sure that scopes have been restored
            auto &currScope = GetCurrentScope();
            bool shouldSliceOuter = currScope[PREVIOUS_SCOPE_KEY] != nullptr;
            if (shouldSliceOuter)
                while (true) // Cringe fast code
                {
                    auto &scope = GetCurrentScope();
                    if(scope[PREVIOUS_SCOPE_KEY] == nullptr)
                        break;
                    else
                        PopScope();
                }
            assert(GetCurrentScope()[OUTER_SCOPE_KEY] != nullptr);
            PopScope();
            if (shouldSliceOuter)
            {
                PushSlice();
            }
            return val;
        }
        else
            return _NIL_;
    }
    const Value EvalId(Object &node)
    {
        return _NIL_;
    }
    const Value EvalFuncDef(Object &node)
    {
        //Create closure
        Object* closure = &GetCurrentScope();
        //Get argument names
        Value idlist = _NIL_;        
        if(node[AST_TAG_IDLIST] != nullptr && node[AST_TAG_IDLIST]->GetType() == Value::ObjectType)
            idlist = Eval(*node[AST_TAG_IDLIST]->ToObject_NoConst()); 
        
        //Get Function Name
        std::string id = "$anonymous";
        if (node[AST_TAG_ID] != nullptr)
            id = node[AST_TAG_ID]->ToString();

        // Set argument list as child of ast
        Object &ast = *node[AST_TAG_BLOCK]->ToObject_NoConst();
        ast.Set(AST_TAG_ARGUMENT_LIST, idlist);
        // Create Function Value
        Value *functionVal = new Value(ast, closure);
        // Add function value to current scope
        auto &scope = GetCurrentScope();
        scope.Set(id, *functionVal);
        debug.ObjectPrintChildren(scope);
        // Slice
        PushSlice();

        return *functionVal;
    }
    const Value EvalConst(Object &node)
    {
        assert(false && "Const shouldnt be evaluated, because you cant pass an object as a Const param. All consts are Values");
        return _NIL_;
    }
    const Value EvalIdlist(Object &node)
    {
        Object *idlist = new Object();
        int num = 0;
        FillIdList(node, *idlist, num);
        debug.ObjectPrintChildren(*idlist);
        if (num > 0)
            return Value(*idlist);
        else
            return _NIL_;
    }
    void FillIdList(Object &node, Object &idlist, int &number)
    {
        if ((node[AST_TAG_IDLIST] == nullptr || node[AST_TAG_IDLIST]->GetType() == Value::NilType) && node[AST_TAG_ID] == nullptr)
            return;
        else if (node[AST_TAG_IDLIST] == nullptr || node[AST_TAG_IDLIST]->GetType() == Value::NilType)
        {
            idlist.Set(number++, node[AST_TAG_ID]->ToString());
            return;
        }
        else
        {
            FillIdList(*node[AST_TAG_IDLIST]->ToObject_NoConst(), idlist, number);
            idlist.Set(number++, node[AST_TAG_ID]->ToString());
            return;
        }
    }
    const Value EvalBreak(Object &node)
    {
        throw BreakException();
        return _NIL_;
    }
    const Value EvalContinue(Object &node) { throw ContinueException(); 
    }
    const Value EvalWhile(Object &node)
    {
        PushNested();
        // Get stmts from stmt -> block -> '{' stmts '}' in whilenode of parser
        Object *whilestmts;
        if (node[AST_TAG_WHILE_STMT]->GetType() == Value::ObjectType && (*node[AST_TAG_WHILE_STMT]->ToObject_NoConst())[AST_TAG_BLOCK] != nullptr)
            if ((*node[AST_TAG_WHILE_STMT]->ToObject_NoConst())[AST_TAG_BLOCK]->GetType() != Value::NilType)
            {
                whilestmts = (*((*node[AST_TAG_WHILE_STMT]->ToObject_NoConst())[AST_TAG_BLOCK]->ToObject_NoConst()))[AST_TAG_STMTS]->ToObject_NoConst();
            }
            else
                whilestmts = node[AST_TAG_WHILE_STMT]->ToObject_NoConst();

        // Execute while
        while (Eval(*node[AST_TAG_WHILE_COND]->ToObject_NoConst()))
            try
            {
                if (node[AST_TAG_WHILE_STMT]->GetType() != Value::NilType)
                {
                    Eval(*node[AST_TAG_WHILE_STMT]->ToObject_NoConst());

                    // EvalStmts(*whilestmts);
                }
            }
            catch (const BreakException &)
            {
                break;
            }
            catch (const ContinueException &)
            {
                continue;
            } // redundant
        PopScope();
        return _NIL_;
    }
    const Value EvalFor(Object &node)
    {
        PushNested();
        // Get stmts from stmt -> block -> '{' stmts '}' in fornode of parser
        Object *forstmts;
        if (node[AST_TAG_FORSTMT]->GetType() == Value::ObjectType && (*node[AST_TAG_FORSTMT]->ToObject_NoConst())[AST_TAG_BLOCK] != nullptr)
            if ((*node[AST_TAG_FORSTMT]->ToObject_NoConst())[AST_TAG_BLOCK]->GetType() != Value::NilType)
            {
                forstmts = (*((*node[AST_TAG_FORSTMT]->ToObject_NoConst())[AST_TAG_BLOCK]->ToObject_NoConst()))[AST_TAG_STMTS]->ToObject_NoConst();
            }
            else
                forstmts = node[AST_TAG_FORSTMT]->ToObject_NoConst();

        // Execute for
        if (node[AST_TAG_INIT]->GetType() != Value::NilType)
            EvalElist(*node[AST_TAG_INIT]->ToObject_NoConst());

        for (; Eval(*node[AST_TAG_EXPR]->ToObject_NoConst());)
        {
            try{
                if (node[AST_TAG_FORSTMT]->GetType() != Value::NilType)
                {
                    // EvalStmts(*forstmts);
                    Eval(*node[AST_TAG_FORSTMT]->ToObject_NoConst());
                }
            }
            catch (const BreakException &)
            {
                break;
            }
            catch (const ContinueException &)
            {
                continue;
            } // redundant
            

            if (node[AST_TAG_FORCOND]->GetType() != Value::NilType)
                EvalElist(*node[AST_TAG_FORCOND]->ToObject_NoConst());
        }
        PopScope();
        return _NIL_;
    }
    const Value EvalIf(Object &node)
    {
        PushNested();
        if (dispatcher->Eval(*node[AST_TAG_EXPR]->ToObject_NoConst()) && node[AST_TAG_IF_STMT]->GetType() != Value::NilType)
            dispatcher->Eval(*node[AST_TAG_IF_STMT]->ToObject_NoConst());
        else if (auto *elseStmt = node[AST_TAG_ELSE_STMT])
        {
            dispatcher->Eval(*elseStmt->ToObject_NoConst());
        }
        PopScope();
        return _NIL_;
    }
    const Value EvalReturn(Object &node)
    {
        if(node[AST_TAG_EXPR] != nullptr){
            RETVAL_SET(Eval(*node[AST_TAG_EXPR]->ToObject_NoConst()));
        }
        else if(node[AST_TAG_EXPR] == nullptr)
            RETVAL_SET(_NIL_);
        else
            assert(false && "Not supported type of return");

        throw ReturnException();
    }
    const Value EvalObjGet(Object &node)
    {
        // FIX when call is done
        debug.ObjectPrintChildren(node, node[AST_TAG_LINE_KEY]->Stringify()), node[AST_TAG_TYPE_KEY]->Stringify(); 
        std::string disambiguate = node[AST_TAG_DISAMBIGUATE_OBJECT]->Stringify();
        const Value lvalue = Eval(*node[AST_TAG_LVALUE]->ToObject_NoConst());

        std::string index;
        if (disambiguate == ".id")
        {
            // index = Eval(*node[AST_TAG_ID]->ToObject_NoConst()).Stringify();
            index = node[AST_TAG_ID]->Stringify();
        }
        else if (disambiguate == "[expr]")
        {
            index = Eval(*node[AST_TAG_EXPR]->ToObject_NoConst()).Stringify();
        }
        else
            assert(false && "Impossible");

        if(lvalue.GetType() == Value::ProgramFunctionType){
            if(index != "$closure")
                assert(false && "Only $closure is accepted on functions");
            debug.ObjectPrintChildren((*lvalue.ToProgramFunctionClosure_NoConst()));
            return Value(*lvalue.ToProgramFunctionClosure_NoConst());
        }
        else{
            return *(*lvalue.ToObject_NoConst())[index];
        }
    }
    const Value &EvalObjSet(Object &node)
    {
        // Same as ObjGet but returns ref to be changed in assignexpr
        std::string disambiguate = node[AST_TAG_DISAMBIGUATE_OBJECT]->Stringify();
        const Value &lvalue = EvalLvalue(*node[AST_TAG_LVALUE]->ToObject_NoConst());
        std::string index;
        if (disambiguate == ".id")
        {
            // index = Eval(*node[AST_TAG_ID]->ToObject_NoConst()).Stringify();
            index = node[AST_TAG_ID]->Stringify();
        }
        else if (disambiguate == "[expr]")
        {
            index = Eval(*node[AST_TAG_EXPR]->ToObject_NoConst()).Stringify();
        }
        else
            assert(false && "Impossible");

        if(lvalue.GetType() == Value::ProgramFunctionType){
            if(index != "$closure")
                assert(false && "Only $closure is accepted on functions");

            return lvalue;
        }
        else{
            if ((*lvalue.ToObject_NoConst())[index] == nullptr)
                lvalue.ToObject_NoConst()->Set(index, Value(_NIL_));

            return *(*lvalue.ToObject_NoConst())[index];
        }

    }
    const Value EvalObjSetWithValue(Object &node, const Value &value)
    {
        return _NIL_;
    }
    const Value &EvalLvalue(Object &node, bool allowDefinition = false)
    {

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
                    debug.LookupPrint("Lookup Lvalue: found " + lookup->Stringify());
                    return *lookup;
                }
                else if (allowDefinition)
                {
                    debug.LookupPrint("Lookup Lvalue: not_found");
                    auto &scope = GetCurrentScope();
                    debug.ObjectPrintChildren(scope);
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
                {
                    debug.LookupPrint("Local lookup result: found");
                    return *lookup;
                }
                else
                {
                    // Can it be local id in rvalue?
                    auto &scope = GetCurrentScope();
                    scope.Set(id, *(new Value()));
                    return *(scope[id]);
                };
            }
            else if (disambiguate == "doubledots id")
            {
                Object *curr_scope = &GetCurrentScope();
                while ((*curr_scope)[OUTER_SCOPE_KEY])
                {
                    curr_scope = (*curr_scope)[OUTER_SCOPE_KEY]->ToObject_NoConst();
                }
                const Value *lookup = ScopeLookup(*curr_scope, id);
                if (lookup)
                {
                    debug.LookupPrint("Lookup Rvalue: found " + lookup->Stringify());
                    return *lookup;
                }
                else
                {
                    curr_scope->Set(id, *(new Value()));
                    return *((*curr_scope)[id]);
                }
            }
        }
        else if (node[AST_TAG_MEMBER])
        {
            return EvalObjSet(*(node[AST_TAG_MEMBER]->ToObject_NoConst()));
        }
        else
            assert(false && "Not implemented yet");
        assert(false);
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
        std::string parentKey = (GetCurrentScope().children.find(OUTER_SCOPE_KEY) != GetCurrentScope().children.end())
                                    ? OUTER_SCOPE_KEY
                                    : PREVIOUS_SCOPE_KEY;
        Object *poped = &GetCurrentScope();
        SetCurrentScope(new Value(GetCurrentScope().children[parentKey]));
        return poped;
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
        SetCurrentScope(new Value(*closure));
        envStack->Top().ToObject_NoConst()->GetAndRemove(CLOSURE_SCOPE_KEY);
        envStack->Top().ToObject_NoConst()->Set(CLOSURE_SCOPE_KEY, new Value(closure));
    }
    void PopScopeSpace()
    {
        envStack->Pop();
    }
    Object &GetCurrentScopeSpace(){
        return *envStack->Top().ToObject_NoConst();
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
        dispatcher->Install(AST_TAG_CALL, [this](Object &node)
                            { return EvalCall(node); });
        dispatcher->Install(AST_TAG_CALLSUFFIX, [this](Object &node)
                            { return EvalCallSuffix(node); });
        dispatcher->Install(AST_TAG_NORMCALL, [this](Object &node)
                            {return EvalNormCall(node);});
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
        dispatcher->Install(AST_TAG_INDEXEDELEM, [this](Object &node)
                            { return EvalIndexedElem(node); });
        dispatcher->Install(AST_TAG_ID, [this](Object &node)
                            { return EvalId(node); });
        dispatcher->Install(AST_TAG_IDLIST, [this](Object &node)
                            { return EvalIdlist(node); });
        dispatcher->Install(AST_TAG_MEMBER, [this](Object &node)
                            { return EvalMember(node); });
    }


    //Library Functions
    static void Print_LibFunc(Object& env){
        int i = 0;
        while(env[i]){
            if(env[i]->GetType() == Value::ObjectType){
                // PRINT_BLUE_LINE(ObjectToString(env[i]->ToObject_NoConst(), "", ""));
                NORMAL_PRINT_LINE(ObjectToString(env[i]->ToObject_NoConst(), "", "")); //If colors bug the terminal
            }
                
            else{
                // PRINT_BLUE_LINE(env[i]->Stringify());
                NORMAL_PRINT_LINE(env[i]->Stringify()); //If colors bug the terminal
            }
                
            i++;
        }       
    }

public:
    Interpreter(bool isDebugMode)
    {
        debug.SetEnabled(isDebugMode);
        dispatcher = new EvalDispatcher();

        envStack = new ValueStack();
        PushScopeSpace();
        PushNewScope();  
        GetCurrentScope().Set("print", *(new Value((LibraryFunc)&this->Print_LibFunc)));
        debug.ValueStackDebugPrint(envStack);
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
