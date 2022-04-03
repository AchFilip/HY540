#define AST_TAG_IF			"if"
#define AST_TAG_IF_COND		"if_cond"
#define AST_TAG_IF_STMT		"if_stmt"
#define AST_TAG_WHILE_COND	"while_cond"
#define AST_TAG_WHILE_STMT	"while_stmt"
#define AST_TAG_WHILE		"while"
#define AST_TAG_FOR			"for"

#pragma once
#include "Object.h"

class TreeVisitor {
	public:
	virtual void VisitVarDecl (const Object& node) {} 
	virtual void VisitFuncDef (const Object& node) {} 
	virtual void VisitBlock   (const Object& node) {} 
	virtual void VisitIf      (const Object& node) {} 
	virtual void VisitWhile   (const Object& node) {} 
	virtual void VisitExpr    (const Object& node) {} 
	virtual void VisitStmt    (const Object& node) {} 
	// ... all the rest visitor methods...

	virtual TreeVisitor* Clone (void) const = 0;
	TreeVisitor(void);
	TreeVisitor (const TreeVisitor&)=default;
};
