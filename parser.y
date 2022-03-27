%{
#include <iostream>
#include <vector>

int yyerror(char const *s);
int yylex(void);

extern int yylineno;
extern char* yytext;
extern FILE* yyin;

void PrintParsing(std::string s1, std::string s2){
    std::cout << s1 << " -> " << s2 << std::endl;
}
%}

%start program


/*  tokens  */
%token IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND OR NOT LOCAL TRUE FALSE NIL INTEGER REAL 

%token  ID STRING

/*  token rules */
%left '(' ')' 
%left '[' ']' 
%left '.' DOUBLEDOTS 
%right NOT PP MM UMINUS
%left '*' '/' '%' 
%left '+' '-'
%nonassoc '>' '<' BRANCHBIGEQ BRANCHSMALLEQ
%nonassoc EQEQ DIF DOUBLECOLON
%left AND
%left OR 
%right '=' 

%%
/*  grammar rules   */
program:        stmts                                            {PrintParsing("program","stmts");}
                ;

stmt:           expr ';'                                         {PrintParsing("stmt","expr");}
                | ifstmt
                | whilestmt
                | forstmt
                | returnstmt
                | BREAK ';'
                | CONTINUE ';'
                | block
                | funcdef
                | ';'                                           {std::cout << "stmt -> ;" << std::endl;}

expr:           assignexpr                                              {std::cout << "expr -> assignexpr" << std::endl;}
                | expr '+' expr                                         {PrintParsing("expr","expr + expr")}
                | expr '-' expr                                         {PrintParsing("expr","expr - expr")}
                | expr '*' expr                                         {PrintParsing("expr","expr * expr")}
                | expr '/' expr                                         {PrintParsing("expr","expr / expr")}
                | expr '%' expr                                         {PrintParsing("expr","expr % expr")}
                | expr '>' expr                                         {PrintParsing("expr","expr > expr")}
                | expr 'BRANCHBIGEQ' expr                               {PrintParsing("expr","expr >= expr")}
                | expr '<' expr                                         {PrintParsing("expr","expr < expr")}
                | expr 'BRANCHSMALLEQ' expr                             {PrintParsing("expr","expr <= expr")}
                | expr 'EQEQ' expr                                      {PrintParsing("expr","expr == expr")}
                | expr 'DIF' expr                                       {PrintParsing("expr","expr != expr")}
                | expr 'AND' expr                                       {PrintParsing("expr","expr and expr")}
                | expr 'OR' expr                                        {PrintParsing("expr","expr or expr")}
                | term                                                  {PrintParsing("expr","term")}
                ;

term:           '(' expr ')'
                | '-' expr
                | NOT lvalue
                | PP lvalue
                | lvalue PP
                | MM lvalue
                | lvalue MM
                | primary
                ;

assignexpr:     lvalue '=' expr                                 {std::cout << "assignexpr -> lvalue = expr"             << std::endl;}
                ;

primary:        lvalue
                | call
                | objectdef
                | '(' funcdef ')'
                | const
                ;

lvalue:         ID                                              {PrintParsing("lvalue","ID");}
                | local ID                                      {PrintParsing("lvalue","local ID");}
                | DOUBLEDOTS ID                                 {PrintParsing("lvalue","DOUBLEDOTS ID");}
                | member                                        {PrintParsing("lvalue","member");}
                ;

call:           call '(' elist ')'
                | lvalue callsuffix
                | '(' funcdef ')' '(' elist ')'
                ;

callsuffix:     normcall
                | methodcall
                ;

normcall:       '(' elist ')'
                ;

methodcall:      DOUBLEDOTS ID '(' elist ')'
                ;

elist:          elist ',' expr                                  {std::cout << "elist -> elist, expr"                    << std::endl;}
                |expr                                           {std::cout << "elist -> expr"                           << std::endl;}
                | /*empty*/                                     {std::cout << "elist -> /*empty*/"                      << std::endl;}
                ;

objectdef:      '['elist']'                                     {std::cout << "objectdef -> [elist]"                    << std::endl;}
                |'['indexed']'                                  {std::cout << "objectdef -> [indexed]"                  << std::endl;}
                ;

indexed:        indexed ',' indexedelem                         {std::cout << "indexed -> indexed, indexedelem"         << std::endl;}
                | indexedelem                                   {std::cout << "indexed -> indexedelem"                  << std::endl;}
                ;

indexedelem:    '{' expr ':' expr '}'                           {std::cout << "indexedelem -> {expr : expr}"            << std::endl;}
                ;

stmts:          stmts stmt                                      {std::cout << "stmts -> stmts stmt"                     << std::endl;}
                |                                               {std::cout << "stmts -> /*empty*/"                      << std::endl;}
                ;

block:          '{' stmts '}'                                   { std::cout << "block -> {stmts}" << std::endl; }
                ;

funcdef:        FUNCTION '('  idlist ')' block                  {std::cout << "funcdef -> FUNCTION (idlist) block"      << std::endl;}
                |  FUNCTION ID '(' idlist ')' block             { std::cout << "funcdef -> FUNCTION ID (idlist) block"   << std::endl;}
                ;

const:          number                                          {std::cout << "const -> number"                         << std::endl;}
                | STRING                                        {std::cout << "const -> STRING"                         << std::endl;}
                | NIL                                           {std::cout << "const -> NIL"                            << std::endl;}
                | TRUE                                          {std::cout << "const -> TRUE"                           << std::endl;}
                | FALSE                                         {std::cout << "const -> FALSE"                          << std::endl;}
                ;

number:         INTEGER                                         {std::cout << "number -> INTEGER"                       << std::endl;}
                | REAL                                          {std::cout << "number -> REAL"                          << std::endl;}
                ;

idlist:         idlist ',' ID                                   {std::cout << "idlist -> idlist, ID"                    << std::endl;}
                |ID                                             {std::cout << "idlist -> ID"                            << std::endl;}                                                                        
                |                                               {std::cout << "idlist -> /*empty*/"                                    << std::endl;}
                ;

ifstmt:         IF '(' expr ')' stmt                            {std::cout << "ifstmt -> IF (expr) stmt"                << std::endl;}
                | IF '(' expr ')' stmt ELSE stmt                {std::cout << "ifstmt -> IF (expr) stmt ELSE stmt"                << std::endl;}
                ;

whilestmt:      WHILE '(' expr ')' stmt                         {std::cout << "whilestmt -> WHILE (expr) stmt"          << std::endl;}
                ;

forstmt:        FOR '(' elist ';' expr ';' elist ')' stmt       {std::cout << "forstmt -> FOR (elist; expr; elist)"     << std::endl;}
                ;

returnstmt:     RETURN ';'                                      {std::cout << "returnstmt -> RETURN;"                   << std::endl;}
                | RETURN expr ';'                               {std::cout << "returnstmt -> RETURN expr;"              << std::endl;}
                ;
                
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


