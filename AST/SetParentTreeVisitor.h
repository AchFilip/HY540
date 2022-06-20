#pragma once
#include "./Object.h"
#include "./TreeVisitor.h"
#include "./ValueStack.h"
#include <iostream>

class SetParentTreeVisitor final: public TreeVisitor{
private:
    ValueStack valueStack;
    bool isRemoval = false;
    void SetParent(const Object& parent){
        if(isRemoval){
            const_cast<Object&>(parent).Remove(PARENT_FIELD);
        }
        else{
            while(!valueStack.IsEmpty()){
                valueStack.GetTopAndPop().ToObject_NoConst()->Set(PARENT_FIELD, parent);
            }
            valueStack.Push(parent);
        }
    }
public:
    virtual TreeVisitor *Clone(void) const {return nullptr;};
    SetParentTreeVisitor(void) = default;
    SetParentTreeVisitor(const SetParentTreeVisitor &) = default;

#define IMPL_VISIT(type) \
virtual void Visit##type (const Object& node) override {SetParent(node);}
    IMPL_VISIT (VarDecl         )
    IMPL_VISIT (Stmts           )
    IMPL_VISIT (Stmt            )
    IMPL_VISIT (Expr            )
    IMPL_VISIT (ArithmeticExpr  )
    IMPL_VISIT (RelationalExpr  )
    IMPL_VISIT (Assignexpr      )
    IMPL_VISIT (Term            )
    IMPL_VISIT (Primary         )
    IMPL_VISIT (Lvalue          )
    IMPL_VISIT (Member          )
    IMPL_VISIT (Call            )
    IMPL_VISIT (CallSuffix      )
    IMPL_VISIT (NormCall        )
    IMPL_VISIT (MethodCall      )
    IMPL_VISIT (Elist           )
    IMPL_VISIT (ObjectDef       )
    IMPL_VISIT (Indexed         )
    IMPL_VISIT (IndexedElem     )
    IMPL_VISIT (Block           )
    IMPL_VISIT (Id              )
    IMPL_VISIT (FuncDef         )
    IMPL_VISIT (Const           )
    IMPL_VISIT (Number          )
    IMPL_VISIT (Idlist          )
    IMPL_VISIT (If              )
    IMPL_VISIT (While           )
    IMPL_VISIT (For             )
    IMPL_VISIT (Return          )
    IMPL_VISIT (Break           )
    IMPL_VISIT (Continue        )
    IMPL_VISIT (QuasiQuotes     )
    IMPL_VISIT (Escape          )
#undef IMPL_VISIT

    void SetIsRemoval (bool v) { isRemoval = v; }
};