#pragma once
#include <functional>
#include <map>
#include "Value.h"
#include "./TreeTags.h"


class Object {
	public:
	Object (void) = default;
	Object (const Object& other);
	~Object();
	
	AST_TAG 						ast_tag;
	std::map<std::string , Object*> children;
	Value*							value;

	using Applier = std::function<void(const Value& key, Value& val)>;
	using Visitor = std::function<void(const Value& key, const Value& val)>;
	void		    IncRefCounter (void);
	void		    DecRefCounter (void);
	const Value*	operator[](const std::string& key) const; // null=not found
	const Value*	operator[](double key) const; // null=not found
			// read and remove access: special purpose
	const Value	    GetAndRemove (const std::string& key);
	const Value	    GetAndRemove (double key);

    void		    Set (const std::string& key, const Value& value);
    void		    Set (double key, const Value& value);
    void		    Remove (const std::string& key);
    void		    Remove (double key);

	unsigned		GetTotal (void) const;
	void		    Apply (const Applier& f); // RW access
	void		    Visit (const Visitor& f) const; // RO access

	void AddChild(std::string ast_tag, Object* child){
		children.insert({ast_tag, child});
	}

	// Do we need those here or not? 
	//Object* Value::ToObject_NoConst (void) const;
    //Object* Value::ToProgramFunctionAST_NoConst (void) const;
    //Object* Value::ToProgramFunctionClosure_NoConst (void) const;
};