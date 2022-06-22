#pragma once
#include <functional>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include "./TreeTags.h"
#include "Value.h"

class Object
{
private:
    int refCounter = 0;

public:
    Object(void) = default;
    Object(const Object &other);
    ~Object() = default;

    std::map<std::string, const Value> children;
    std::vector<std::string> childrenTags;
    
    using Applier = std::function<void(const Value &key, Value &val)>;
    using Visitor = std::function<void(const Value &key, const Value &val)>;
    void IncRefCounter(void)
    {
        ++refCounter;
    }
    void DecRefCounter(void)
    {
        refCounter > 0 ? --refCounter : refCounter;
        // std::cout << refCounter << std::endl;
        if (refCounter == 0)
        {
            // std::cout << "Deleting " << std::endl;
            this->~Object();
        }
    }
    const Value *operator[](const std::string &key) const
    {
        if (children.find(key) != children.end())
            return &(children.at(key));
        else
            return nullptr;
    }
    const Value *operator[](double key) const
    {
        if (children.find(std::to_string(key)) != children.end())
            return &(children.at(std::to_string(key)));
        else
            return nullptr;
    }

    // read and remove access: special purpose
    Value GetAndRemove(const std::string &key)
    {
        if (children.find(key) != children.end())
        {
            Value returnVal = children[key];
            children.erase(key);
            return returnVal;
        }
        else
        {
            return Value(_NIL_);
        }
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
    void Remove(const std::string &key)
    {
        if (children.find(key) != children.end())
            children.erase(key);
        return;
    }
    void Remove(double key)
    {
        if (children.find(std::to_string(key)) != children.end())
            children.erase(std::to_string(key));
        return;
    };

    unsigned GetTotal(void) const;
    void Apply(const Applier &f);       // RW access
    void Visit(const Visitor &f) const; // RO access

    std::string ToString() const
    {
        return "";
    }
    void Debug_PrintChildren() const
    {

        for (auto iter = children.begin(); iter != children.end(); ++iter)
        {
            std::cout << "\t[" << iter->first << "," << iter->second.Stringify() << "]" << std::endl;
        }
    }
};
