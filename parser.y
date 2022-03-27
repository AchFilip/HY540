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

stmts:          stmts stmt                                      {PrintParsing("stmts","stmt");}
                |                                               {PrintParsing("stmts", "empty");}
                ;

block:          '{' stmts '}'                                   {PrintParsing("block", "stmts");}
                ;

funcdef:        FUNCTION '('  idlist ')' block                  {PrintParsing("funcdef", "FUNCTION (idlist) block");}
                |  FUNCTION ID '(' idlist ')' block             {PrintParsing("funcdef", "FUNCTION ID (idlist) block");}
                ;

const:          number                                          {PrintParsing("const", "number");}
                | STRING                                        {PrintParsing("const", "STRING");}
                | NIL                                           {PrintParsing("const", "NIL");}
                | TRUE                                          {PrintParsing("const", "TRUE");}
                | FALSE                                         {PrintParsing("const", "FALSE");}
                ;

number:         INTEGER                                         {PrintParsing("number", "INTEGER");}
                | REAL                                          {PrintParsing("number", "REAL");}
                ;

idlist:         idlist ',' ID                                   {PrintParsing("idlist", "idlist , ID");}
                |ID                                             {PrintParsing("idlist", "ID");}                                                                        
                |                                               {PrintParsing("idlist", "empty");}
                ;

ifstmt:         IF '(' expr ')' stmt                            {PrintParsing("ifstmt", "IF ( expr ) stmt");}
                | IF '(' expr ')' stmt ELSE stmt                {PrintParsing("ifstmt", "IF ( expr ) stmt ELSE stmt");}
                ;

whilestmt:      WHILE '(' expr ')' stmt                         {PrintParsing("whilestmt", "WHILE ( expr ) stmt");}
                ;

forstmt:        FOR '(' elist ';' expr ';' elist ')' stmt       {PrintParsing("forstmt", "FOR ( elist ; expr ; elist ) stmt");}
                ;

returnstmt:     RETURN ';'                                      {PrintParsing("returnstmt", " RETURN ;");}
                | RETURN expr ';'                               {PrintParsing("returnstmt", " RETURN expr ;");}
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


