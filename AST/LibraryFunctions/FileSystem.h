#pragma once
#include <iostream>
#include <fstream>
#include <functional>
#include "../TreeTags.h"
#include "../Object.h"
#include "./Utilities.h"
#include "../EvalDispatcher.h"
#include "../DebugAST.h"

class FileSystem
{
public:
    
    // arg1: path string
    // arg2: text string
    // arg3 -optional- : concatenate bool
    static void WriteFile(Object &env)
    {
        std::ios_base::openmode mode = std::ios_base::out;
        //if more than 3 inputs std::cerr
        if (env[0]->GetType() != Value::StringType)
            std::cerr << "First argument is the path and must be string" << std::endl;
        if (env[1]->GetType() != Value::StringType && env[1]->GetType() != Value::ObjectType)
            std::cerr << "First argument is the text and must be string" << std::endl;
        if (env[2] != nullptr)
            if (env[2]->GetType() != Value::BooleanType)
                std::cerr << "Second argument must be boolean" << std::endl;
            else
                mode = std::ios_base::app;

        std::string path = env[0]->Stringify();
        std::string text;
        
        env[1]->GetType() == Value::ObjectType ? text = Utilities::ObjectToString(env[1]->ToObject_NoConst(), "", "") : text = env[1]->Stringify();

        std::ofstream MyFile(path, mode);
        MyFile << "\n" << text;
        MyFile.close();
    }
    
    // arg1: path string
    static void ReadFile(Object &env){
        //if more than 2 inputs std::cerr
        
        if (env[0]->GetType() != Value::StringType)
            std::cerr << "First argument is the path and must be string";
        
        std::string path = env[0]->Stringify();
        std::string text;
        std::ifstream MyReadFile(path);

        while (std::getline (MyReadFile, text)) {
            // Output the text from the file
            std::cout << text;
        }
        std::cout << "\n";
    }
};