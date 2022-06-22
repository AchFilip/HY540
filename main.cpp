#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "./AST/SetParentTreeVisitor.h"
#include "./AST/Debug/SinDebugger.h"
#include "./AST/Debug/DebugMessageInterface.h"
#include "./parser.cpp"
#include "./AST/Sin_Eval.h"

void Unparse(Object &ast)
{
    TreeHost *treeHost = new TreeHost();
    treeHost->Accept(new UnparseTreeVisitor(), ast);
    std::cout << "AST: " << ast[UNPARSE_VALUE]->ToString() << std::endl;
    std::ofstream MyFile("code.alpha");
    MyFile << ast[UNPARSE_VALUE]->ToString();
    MyFile.close();
}

Object &SetParent(Object &ast)
{
    TreeHost *treeHost = new TreeHost();
    treeHost->Accept(new SetParentTreeVisitor(), ast);
    return ast;
}

void Interpret(Object &ast)
{
    Interpreter *interpreter = new Interpreter(false);
    Sin_Eval *sineval = new Sin_Eval();
    sineval->SetInterpreter(interpreter);
    eval_LanguageIface::Set(sineval);
    interpreter->StartProgram(ast);
    delete interpreter;
}

std::string FileToString(std::string fileName)
{
    std::string fileContent;
    std::getline(std::ifstream(fileName), fileContent, '\0');
    return fileContent;
}

int main(int argc, char *argv[])
{
    // Run from file
    if (argc == 2)
    {
        SinDebugger::isDebug = false;

        Parser parser;
        Interpret(SetParent(*parser.Parse(FileToString(argv[1]))));
    }
    // Run from file with debugger
    else if (argc == 3)
    {
        SinDebugger::isDebug = true;
        DebugMessageInterface::Init();

        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            Parser parser;
            Interpret(SetParent(*parser.Parse(FileToString(argv[2]))));
        }
        else if (child_pid > 0)
        {
            SinDebugger debugger;
            debugger.InitDebuggerEnd();
        }
        else
        {
            perror("fork");
            return -1;
        }
    }
}

// ./sin.exe fileName
// ./sin.exe -d fileName

// MESSAGE START
// MESSAGE
// MESSAGE END