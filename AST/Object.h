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
    const Object *operator[](const std::string &key) const {
        if (children.find(key) != children.end())
            return children[key];
        else
            return nullptr;
    };
    const Object *operator[](double key) const {
        return this[to_string(key)]
    };
                                                // read and remove access: special purpose
    const Object GetAndRemove(const std::string &key);
    const Object GetAndRemove(double key);

    void Set(const std::string &key, const Object &value);
    void Set(double key, const Object &value);
    void Remove(const std::string &key);
    void Remove(double key);

    unsigned GetTotal(void) const;
    void Apply(const Applier &f);       // RW access
    void Visit(const Visitor &f) const; // RO access

    void AddChild(std::string ast_tag, Object *child)
    {
        children.insert({ast_tag, child});
    }
    Object* GetChild(std::string ast_tag){
        return children[ast_tag];
    }

    void RecursivePrint(int tabs)
    {
        // TODO: Print every children recursively
        std::cout << std::setw(4 * tabs);
        std::cout << "--" << this->ast_tag << std::endl;
        // Reverse traverse of the tree or else it starts from the last command
        for (auto child = this->children.rbegin(); child != this->children.rend(); child++)
        {
            child->second->RecursivePrint(tabs + 1);
        }
    }

    static void CreateGraph(Object *root)
    {
        std::string graph_text = "graph AST {\n";
        graph_text += "\t" + root->ast_tag + " [label=" + root->ast_tag + "]\n";
        graph_text += GraphVisitChildren(root, 0);
        graph_text += "}";
        // std::cout << graph_text << std::endl;
        std::ofstream graph_file;
        graph_file.open("ast.gv");
        graph_file << graph_text;
        graph_file.close();
    }

    static std::string GraphVisitChildren(Object *node, int cnt)
    {
        // TODO: check again
        std::string text = "";
        // for (auto child : node->children)
        for (auto child = node->children.rbegin(); child != node->children.rend(); child++)
        {
            text += "\t" + child->second->ast_tag + std::to_string(cnt) + " [label=" + child->second->ast_tag + "]\n";
            if (cnt == 0)
                text += "\t" + node->ast_tag + " -- " + child->second->ast_tag + std::to_string(cnt) + "\n";
            else
                text += "\t" + node->ast_tag + std::to_string(cnt - 1) + " -- " + child->second->ast_tag + std::to_string(cnt) + "\n";
            text += GraphVisitChildren(child->second, cnt + 1);
            if (child->second->children.empty())
            {
                if (child->second->value)
                {
                    text += "\tvalue" + std::to_string(cnt) + " [label=" + child->second->value->ToString() + "]\n";
                    text += "\t" + child->second->ast_tag + std::to_string(cnt) + " -- value" + std::to_string(cnt) + "\n";
                }
            }
        }
        return text;
    }

    // Do we need those here or not?
    // Object* Value::ToObject_NoConst (void) const;
    // Object* Value::ToProgramFunctionAST_NoConst (void) const;
    // Object* Value::ToProgramFunctionClosure_NoConst (void) const;
};