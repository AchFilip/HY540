//#include <functional>
#pragma once
#include "TreeVisitor.h"
#include <map>

class TreeHost {
	private:
	using Acceptor  = std::function<void(const Object&)>;
	using Acceptors = std::map<std::string, Acceptor>;

	Acceptors    acceptors;
	TreeVisitor* visitor = nullptr;

	void Accept (const Object& node)
		{ acceptors[node[AST_TAG_TYPE_KEY]->ToString()](node); }

	void AcceptIf (const Object& node) {
		Accept(*node[AST_TAG_IF_COND]->ToObject());
		Accept(*node[AST_TAG_IF_STMT]->ToObject());
		visitor->VisitIf(node);
	}

	void AcceptWhile (const Object& node) {
		Accept(*node[AST_TAG_WHILE_COND]->ToObject());
		Accept(*node[AST_TAG_WHILE_STMT]->ToObject());
		visitor->VisitWhile(node);
	}

	// ... all the rest tag-specific (node-type specific) accept methods ...
	void InstallAcceptor (const std::string& tag, const Acceptor& f)
		{ acceptors[tag] = f ; }

	void InstallAllAcceptors (void) {
		InstallAcceptor(AST_TAG_IF,[this](const Object& node) 
			{ AcceptIf(node); });
		InstallAcceptor(AST_TAG_WHILE,[this](const Object& node) 
			{ AcceptWhile(node); });
	}

	public:
	void Accept (TreeVisitor* _visitor, const Object& node) // post order visit
		{ visitor = _visitor; Accept(node); }
};
