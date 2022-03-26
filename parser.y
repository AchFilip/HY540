%{
#include <iostream>
#include <vector>

int yyerror(char const *s);
int yylex(void);

extern int yylineno;
extern char* yytext;
extern FILE* yyin;
%}

%start program

/*  tokens  */
%token  INTEGER


/*  token rules */
%right  '='
%left   '+' '-'

%%
/*  grammar rules   */
program:        expression 
                ;

expression:     INTEGER {std::cout << "integer -> expression" << std::endl;}
%%

int yyerror(char const *s){
    std::cout << s << " in line: "<< yylineno << std::endl;
    exit(0);
}

int main(int argc, char** argv){
    if(argc>1){
        if(!(yyin=fopen(argv[1],"r"))){
            fprintf(stderr,"cannto access");
            return 1;
        }
    }else{
        yyin=stdin;
    }

    yyparse();
    return 0;
}


