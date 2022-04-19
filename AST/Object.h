#pragma once
#include <functional>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>

#include "./TreeTags.h"
#include "Value.h"

class Object
{
public:
    Object(void) = default;
    Object(const Object &other);
    ~Object();

    AST_TAG ast_tag;
    std::map<std::string, Object *> children;
    Value *value;

    using Applier = std::function<void(const Value &key, Value &val)>;
    using Visitor = std::function<void(const Value &key, const Value &val)>;
    void IncRefCounter(void);
    void DecRefCounter(void);
    const Value *operator[](const std::string &key) const; // null=not found
    const Value *operator[](
        double key) const; // null=not found
                           // read and remove access: special purpose
    const Value GetAndRemove(const std::string &key);
    const Value GetAndRemove(double key);

    void Set(const std::string &key, const Value &value);
    void Set(double key, const Value &value);
    void Remove(const std::string &key);
    void Remove(double key);

    unsigned GetTotal(void) const;
    void Apply(const Applier &f);       // RW access
    void Visit(const Visitor &f) const; // RO access

    void AddChild(std::string ast_tag, Object *child)
    {
        children.insert({ast_tag, child});
    }

    void RecursivePrint(int tabs)
    {
        // TODO: Print every children recursively
        std::cout << std::setw(4 * tabs);
        std::cout << "--" << this->ast_tag << std::endl;
        for (auto child : this->children)
        {
            child.second->RecursivePrint(tabs + 1);
        }
    }

    static void CreateGraph(Object *root)
    {
        std::string graph_text = "graph AST {\n";
        graph_text += GraphVisitChildren(root, 0);
        graph_text += "}";
        std::cout << graph_text << std::endl;
        std::ofstream graph_file;
        graph_file.open("ast.gv");
        graph_file << graph_text;
        graph_file.close();
    }

    static std::string GraphVisitChildren(Object *node, int cnt)
    {
        std::string text = "";
        if (cnt == 0)
            text += "\t" + node->ast_tag + std::to_string(cnt) + " [label=" + node->ast_tag + "]\n";
        for (auto child : node->children)
        {
            text += "\t" + child.second->ast_tag + std::to_string(cnt) + " [label=" + child.second->ast_tag + "]\n";
            if (cnt == 0)
                text += "\t" + node->ast_tag + std::to_string(cnt) + " -- " + child.second->ast_tag + std::to_string(cnt) + "\n";
            else
                text += "\t" + node->ast_tag + std::to_string(cnt - 1) + " -- " + child.second->ast_tag + std::to_string(cnt) + "\n";
            text += GraphVisitChildren(child.second, cnt + 1);
        }
        return text;
    }
    // Do we need those here or not?
    // Object* Value::ToObject_NoConst (void) const;
    // Object* Value::ToProgramFunctionAST_NoConst (void) const;
    // Object* Value::ToProgramFunctionClosure_NoConst (void) const;
};