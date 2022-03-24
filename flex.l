%top{
#include <iostream>
#include <vector>

struct aplha_token_t{
    int numLine;
    int numToken;
    std::string value;
    std::string type;
    std::string content;   
};

#define YY_DECL int a_yylex(void* yylval)

}

%option noyywrap
%option yylineno

ID             [0-9]+

%%

{ID}           {std::cout<<"Found Integer: " << yytext;}

%%

int main(int argc, char** argv){
    std::vector<aplha_token_t>* myTokens = new std::vector<aplha_token_t>;
    if(argc>1){
        if(!(yyin=fopen(argv[1],"r"))){
            fprintf(stderr,"cannto access");
            return 1;
        }
    }else{
        yyin=stdin;
    }

    a_yylex(myTokens);
    return 0;
}
