#include <iostream>
#include <fstream>
#include "./AST/SetParentTreeVisitor.h"
#include "./parser.cpp"

void Unparse(Object& ast){
    TreeHost *treeHost = new TreeHost(); 
    treeHost->Accept(new UnparseTreeVisitor(), ast); 
    std::cout << "AST: " << ast[UNPARSE_VALUE]->ToString() << std::endl;
    std::ofstream MyFile("code.alpha");
    MyFile << ast[UNPARSE_VALUE]->ToString();
    MyFile.close();
}

Object& SetParent(Object& ast){
    TreeHost *treeHost = new TreeHost(); 
    treeHost->Accept(new SetParentTreeVisitor(), ast); 
    return ast;
}

void Interpret(Object& ast){
    Interpreter* interpreter = new Interpreter(false);
    interpreter->StartProgram(ast);
    delete interpreter;
}

int main(int argc, char *argv[])
{
    Parser parser;

    if (argc > 1)
    {
        std::string file_content;
        std::getline(std::ifstream(argv[1]), file_content, '\0');
        Interpret(SetParent(*parser.Parse(file_content)));
    }
    else
    {
        std::string final;
        std::string input;
        std::cin >> input;
        while(input != "END"){
            final += input;    
            std::cin >> input;
        }
        Interpret(*parser.Parse(final));
    }

}

