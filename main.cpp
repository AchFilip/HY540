#include <iostream>
#include <fstream>
#include "./parser.cpp"

int main(int argc, char *argv[])
{
    Parser parser;

    if (argc > 1)
    {
        std::string file_content;
        std::getline(std::ifstream(argv[1]), file_content, '\0');
        parser.Parse(file_content);
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
        parser.Parse(final);
    }

}

