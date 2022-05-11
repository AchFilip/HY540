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

    std::map<std::string, const Value> children;

    using Applier = std::function<void(const Value &key, Value &val)>;
    using Visitor = std::function<void(const Value &key, const Value &val)>;
    void IncRefCounter(void);
    void DecRefCounter(void);
    const Value* operator[](const std::string &key) const
    {
        if (children.find(key) != children.end())
            return &(children.at(key));
        else
            return nullptr;
    }
    const Value* operator[](double key) const
    {
        return &(children.at(std::to_string(key)));
    }

    // read and remove access: special purpose
    Value GetAndRemove(const std::string &key)
    {
        Value to_return = children[key];
        auto it = children.find(key);
        if (it != children.end()) 
            children.erase(it);
        return to_return;
    }
    Value GetAndRemove(double key)
    {
        return GetAndRemove(std::to_string(key));
    }

    void Set(const std::string &key, const Value &value)
    {
        children.insert({key, value});
    };
    void Set(double key, const Value &value)
    {
        Set(std::to_string(key), value);
    };
    void Remove(const std::string &key);
    void Remove(double key);

    unsigned GetTotal(void) const;
    void Apply(const Applier &f);       // RW access
    void Visit(const Visitor &f) const; // RO access

    std::string ToString() const{
        return "";
    }
/* COMMENTED OUT METHODS FOR PRINTING 

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

*/
    

    // Do we need those here or not?
    // Object* Value::ToObject_NoConst (void) const;
    // Object* Value::ToProgramFunctionAST_NoConst (void) const;
    // Object* Value::ToProgramFunctionClosure_NoConst (void) const;
};