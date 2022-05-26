#pragma once
#include "./Object.h"
#include "./Value.h"
#include "./ValueStack.h"

class DebugAST{
private: 
    bool isEnabled;
public:
    void SetEnabled(bool isEnabled){
        this->isEnabled = isEnabled;
    }

    void ObjectPrintChildren(Object& obj, std::string line = "Not Particular", std::string type = ""){
        if(isEnabled){
            std::cout << "line " << line.substr(0,1) << ": " << type << std::endl;
            for (auto iter = obj.children.begin(); iter != obj.children.end(); ++iter){
                std::cout << "\t{" << iter->first << "," << iter->second.Stringify() << "}" << std::endl;
            }
        }
    }

    void ValueStackDebugPrint(ValueStack* valueStack){
        if(isEnabled)
            valueStack->Debug_Print();
    }

    void PrintLine(std::string toPrint){
        if(isEnabled)
            std::cout << toPrint << std::endl;
    }

    void LookupPrint(std::string toPrint){
        if(isEnabled){
            std::cout << "Lookup Print" << std::endl;
            std::cout << "\t" << toPrint << std::endl;
        }
            
    }

        
};
