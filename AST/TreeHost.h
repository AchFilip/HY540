//#include <functional>
#pragma once
#include "TreeVisitor.h"
#include <map>

class TreeHost
{
private:
	using Acceptor = std::function<void(const Object &)>;
	using Acceptors = std::map<std::string, Acceptor>;

	Acceptors acceptors;
	TreeVisitor *visitor = nullptr;

	void Accept(const Object &node)
	{
		acceptors[node->ast_tag](node);
	}

	void AcceptStmt(const Object &node)
	{
		if (node["$child"] != nullptr)
			Accept(*node["$child"]);
		visitor->VisitStmt(node);
	}
	void AcceptExpr(const Object &node)
	{
		if (node["$child"] != nullptr)
			Accept(*node["$child"]);
		else
		{
			Accept(*node["$child1"]);
			Accept(*node["$child2"]);
		}
		visitor->VisitExpr(node);
	}
	void AcceptTerm(const Object &node)
	{
		if (node["$child"] != nullptr)
			Accept(*node["$child"]);
		else
		{
			Accept(*node["$child1"]);
			Accept(*node["$child2"]);
		}
		visitor->VisitTerm(node);
	}
	void AcceptAssignexpr(const Object &node)
	{
		Accept(*node["$lvalue"]);
		Accept(*node["$expr"]);
		visitor->VisitAssignexpr(node);
	}
	void AcceptPrimary(const Object &node)
	{
		Accept(*node["$child"]);
		visitor->VisitPrimary(node);
	}
	void AcceptLvalue(const Object &node)
	{
		Accept(*node["$child"]);
		visitor->VisitLvalue(node);
	}
	void AcceptMember(const Object &node)
	{
		Accept(*node["$child1"]);
		Accept(*node["$child2"]);
		visitor->VisitMember(node);
	}
	void AcceptCall(const Object &node)
	{
		Accept(*node["$child1"]);
		Accept(*node["$child2"]);
		visitor->VisitCall(node);
	}
	void AcceptCallSuffix(const Object &node)
	{
		Accept(*node["$child"]);
		visitor->VisitCallSuffix(node);
	}
	void AcceptNormCall(const Object &node)
	{
		Accept(*node["$child"]);
		visitor->VisitNormCall(node);
	}
	void AcceptMethodCall(const Object &node)
	{
		Accept(*node["$child1"]);
		Accept(*node["$child2"]);
		visitor->VisitMethodCall(node);
	}
	void AcceptElist(const Object &node)
	{
		if (node["$child"] != nullptr)
			Accept(*node["$child"]);
		else if (node["$child1"] != nullptr)
		{
			Accept(*node["$child1"]);
			Accept(*node["$child2"]);
		}
		visitor->VisitElist(node);
	}
	void AcceptObjectDef(const Object &node)
	{
		Accept(*node["$child"]);
		visitor->VisitObjectDef(node);
	}
	void AcceptIndexed(const Object &node)
	{
		if (node["$child"] != nullptr)
			Accept(*node["$child"]);
		else
		{
			Accept(*node["$child1"]);
			Accept(*node["$child2"]);
		}
		visitor->VisitIndexed(node);
	}
	void AcceptIndexedElem(const Object &node)
	{
		Accept(*node["$child1"]);
		Accept(*node["$child2"]);
		visitor->VisitIndexedElem(node);
	}
	void AcceptStmts(const Object &node)
	{
		if (node["$child1"] != nullptr)
		{
			Accept(*node["$child1"]);
			Accept(*node["$child2"]);
		}
		visitor->VisitStmts(node);
	}
	void AcceptBlock(const Object &node)
	{
		Accept(*node["$child"]);
		visitor->VisitBlock(node);
	}
	void AcceptId(const Object &node)
	{
		visitor->VisitId(node);
	}
	void AcceptFuncDef(const Object &node)
	{
		if (node["$id"] != nullptr)
			Accept(*node["$id"]);
		Accept(*node["$idlist"]);
		Accept(*node["$block"]);
		visitor->VisitFuncDef(node);
	}
	void AcceptConst(const Object &node)
	{
		visitor->VisitConst(node);
	}
	void AcceptIdlist(const Object &node)
	{
		if (node["$child"] != nullptr)
			Accept(*node["$child"]);
		else if (node["$child1"] != nullptr)
		{
			Accept(*node["$child1"]);
			Accept(*node["$child2"]);
		}
		visitor->VisitIdlist(node);
	}
	void AcceptIf(const Object &node)
	{
		Accept(*node["$ifcond"]);
		Accept(*node["$ifstmt"]);
		if (node["$elsestmt"] != nullptr)
			Accept(*node["$elsestmt"]);
		visitor->VisitIf(node);
	}
	void AcceptWhile(const Object &node)
	{
		Accept(*node["$whilecond"]);
		Accept(*node["$whilestmt"]);
		visitor->VisitWhile(node);
	}
	void AcceptForstmt(const Object &node)
	{
		Accept(*node["$init"]);
		Accept(*node["$cond"]);
		Accept(*node["$expr"]);
		Accept(*node["$stmt"]);
		visitor->VisitFor(node);
	}
	void AcceptReturn(const Object &node)
	{
		visitor->VisitReturn(node);
	}

	// ... all the rest tag-specific (node-type specific) accept methods ...
	void InstallAcceptor(const std::string &tag, const Acceptor &f)
	{
		acceptors[tag] = f;
	}

	void InstallAllAcceptors(void)
	{
		InstallAcceptor(AST_TAG_IF, [this](const Object &node){
			AcceptIf(node); 
		});
		InstallAcceptor(AST_TAG_WHILE, [this](const Object &node){
			AcceptWhile(node); 
		});
	}

public:
	void Accept(TreeVisitor *_visitor, const Object &node) // post order visit
	{
		visitor = _visitor;
		Accept(node);
	}
};
