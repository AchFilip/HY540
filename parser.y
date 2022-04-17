%{
#include "./AST/Object.h"
#include "./AST/Value.h"
#include "./AST/TreeTags.h"
#include <iostream>
#include <vector>

int yyerror(char const *s);
int yylex(void);

extern int yylineno;
extern char* yytext;
extern FILE* yyin;

// Prints the beginning and the end of a translated grammar rule
void PrintParsing(std::string s1, std::string s2){
    std::cout << s1 << " -> " << s2 << std::endl;
}

Object* CreateObject(AST_TAG ast_tag){
    Object* obj = new Object();
    obj->ast_tag = ast_tag;
    return obj;
}
%}

%start program

%union {
    std::string*    strVal;
    double          numVal;
    Object*         objVal;
    struct
    {
        Object *ast;
        Object *closure;
    } progFuncVal;
    struct
    {
        void *ptr;
        char *typeId;
    } nativePtrVal;
}

/*  tokens  */
%token  IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND OR NOT LOCAL TRUE FALSE NIL 
%token  <numVal> NUMBER 
%token  <strVal> ID STRING

%type <objVal> id const primary funcdef idlist block returnstmt expr whilestmt stmt stmts lvalue member call objectdef assignexpr term ifstmt forstmt;

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
program:        stmts                                                   {PrintParsing("program","stmts");}
                ;

stmt:           expr ';'                                                {
                                                                            PrintParsing("stmt","expr ;");
                                                                            Object* obj = CreateObject(AST_TAG_STMT);
                                                                            obj->AddChild("$child", $1);
                                                                            $$ = obj;
                                                                        }
                | ifstmt                                                {
                                                                            PrintParsing("stmt","ifstmt");
                                                                            Object* obj = CreateObject(AST_TAG_STMT);
                                                                            obj->AddChild("$child", $1);
                                                                            $$ = obj;
                                                                        }  
                | whilestmt                                             {
                                                                            PrintParsing("stmt","whiletmt");
                                                                            Object* obj = CreateObject(AST_TAG_STMT);
                                                                            obj->AddChild("$child", $1);
                                                                            $$ = obj;
                                                                        }  
                | forstmt                                               {
                                                                            PrintParsing("stmt","forstmt");
                                                                            Object* obj = CreateObject(AST_TAG_STMT);
                                                                            obj->AddChild("$child", $1);
                                                                            $$ = obj;
                                                                        }  
                | returnstmt                                            {
                                                                            PrintParsing("stmt","returnstmt");
                                                                            Object* obj = CreateObject(AST_TAG_STMT);
                                                                            obj->AddChild("$child", $1);
                                                                            $$ = obj;
                                                                        }  
                | BREAK ';'                                             {
                                                                            PrintParsing("stmt","break ;");
                                                                        }  
                | CONTINUE ';'                                          {
                                                                            PrintParsing("stmt","continue ;");
                                                                        }  
                | block                                                 {
                                                                            PrintParsing("stmt","block");
                                                                            Object* obj = CreateObject(AST_TAG_STMT);
                                                                            obj->AddChild("$child", $1);
                                                                            $$ = obj;
                                                                        }  
                | funcdef                                               {
                                                                            PrintParsing("stmt","funcdef");
                                                                            Object* obj = CreateObject(AST_TAG_STMT);
                                                                            obj->AddChild("$child", $1);
                                                                            $$ = obj;
                                                                        }  
                | ';'                                                   {
                                                                            PrintParsing("stmt",";");
                                                                        }
                ;  

expr:           assignexpr                                              {
                                                                            std::cout << "expr -> assignexpr" << std::endl;
                                                                        }
                | expr '+' expr                                         {
                                                                            PrintParsing("expr","expr + expr");
                                                                        }
                | expr '-' expr                                         {
                                                                            PrintParsing("expr","expr - expr");
                                                                        }
                | expr '*' expr                                         {
                                                                            PrintParsing("expr","expr * expr");
                                                                        }
                | expr '/' expr                                         {
                                                                            PrintParsing("expr","expr / expr");
                                                                        }
                | expr '%' expr                                         {
                                                                            PrintParsing("expr","expr % expr");
                                                                        }
                | expr '>' expr                                         {
                                                                            PrintParsing("expr","expr > expr");
                                                                        }
                | expr BRANCHBIGEQ expr                                 {
                                                                            PrintParsing("expr","expr >= expr");
                                                                        }
                | expr '<' expr                                         {
                                                                            PrintParsing("expr","expr < expr");
                                                                        }
                | expr BRANCHSMALLEQ expr                               {
                                                                            PrintParsing("expr","expr <= expr");
                                                                        }
                | expr EQEQ expr                                        {
                                                                            PrintParsing("expr","expr == expr");
                                                                        }
                | expr DIF expr                                         {
                                                                            PrintParsing("expr","expr != expr");
                                                                        }
                | expr AND expr                                         {
                                                                            PrintParsing("expr","expr and expr");
                                                                        }
                | expr OR expr                                          {
                                                                            PrintParsing("expr","expr or expr");
                                                                        }
                | term                                                  {
                                                                            PrintParsing("expr","term");
                                                                        }
                ;

term:           '(' expr ')'                                            {
                                                                        //Need different name to differntiate between different lvalue/expr
                                                                        //Maybe differnt AST_TAG (??)
                                                                            PrintParsing("term","( expr )");
                                                                            Object* obj = CreateObject(AST_TAG_TERM);
                                                                            obj->AddChild("$parenthesisexpr", $2);
                                                                            $$ = obj;
                                                                        }
                | '-' expr                                              {
                                                                            PrintParsing("term","- expr");
                                                                            Object* obj = CreateObject(AST_TAG_TERM);
                                                                            obj->AddChild("$minusexpr", $2);
                                                                            $$ = obj;
                                                                        }
                | NOT lvalue                                            {
                                                                            PrintParsing("term","not lvalue");
                                                                            Object* obj = CreateObject(AST_TAG_TERM);
                                                                            obj->AddChild("$notlvalue", $2);
                                                                            $$ = obj;
                                                                        }
                | PP lvalue                                             {
                                                                            PrintParsing("term","++ lvalue");
                                                                            Object* obj = CreateObject(AST_TAG_TERM);
                                                                            obj->AddChild("$pplvalue", $2);
                                                                            $$ = obj;
                                                                        }
                | lvalue PP                                             {
                                                                            PrintParsing("term","lvalue ++");
                                                                            Object* obj = CreateObject(AST_TAG_TERM);
                                                                            obj->AddChild("$lvaluepp", $1);
                                                                            $$ = obj;
                                                                        }
                | MM lvalue                                             {
                                                                            PrintParsing("term","-- lvalue");
                                                                            Object* obj = CreateObject(AST_TAG_TERM);
                                                                            obj->AddChild("$mmlvalue", $2);
                                                                            $$ = obj;
                                                                        }
                | lvalue MM                                             {
                                                                            PrintParsing("term","lvalue --");
                                                                            Object* obj = CreateObject(AST_TAG_TERM);
                                                                            obj->AddChild("$lvaluemm", $1);
                                                                            $$ = obj;
                                                                        }
                | primary                                               {
                                                                            PrintParsing("term","primary");
                                                                            Object* obj = CreateObject(AST_TAG_TERM);
                                                                            obj->AddChild("$child", $1);
                                                                            $$ = obj;
                                                                        }
                ;

assignexpr:     lvalue '=' expr                                 {
                                                                    PrintParsing("assignexpr","lvalue = expr");
                                                                    Object* obj = CreateObject(AST_TAG_ASSIGNEXPR);
                                                                    obj->AddChild("$lvalue", $1);
                                                                    obj->AddChild("$expr", $3);
                                                                    $$ = obj;
                                                                }
                ;

primary:        lvalue                                          {
                                                                    PrintParsing("primary","lvalue");
                                                                    Object* obj = CreateObject(AST_TAG_PRIMARY);
                                                                    obj->AddChild("$child", $1);
                                                                    $$ = obj;
                                                                }
                | call                                          {
                                                                    PrintParsing("primary","call");
                                                                    Object* obj = CreateObject(AST_TAG_PRIMARY);
                                                                    obj->AddChild("$child", $1);
                                                                    $$ = obj;
                                                                }
                | objectdef                                     {
                                                                    PrintParsing("primary","objectdef");
                                                                    Object* obj = CreateObject(AST_TAG_PRIMARY);
                                                                    obj->AddChild("$child", $1);
                                                                    $$ = obj;
                                                                }
                | '(' funcdef ')'                               {
                                                                    PrintParsing("primary","( funcdef )");
                                                                    Object* obj = CreateObject(AST_TAG_PRIMARY);
                                                                    obj->AddChild("$child", $2);
                                                                    $$ = obj;
                                                                }
                | const                                         {
                                                                    PrintParsing("primary","const");
                                                                    Object* obj = CreateObject(AST_TAG_PRIMARY);
                                                                    obj->AddChild("$child", $1);
                                                                    $$ = obj;
                                                                }
                ;
                                        
lvalue:         id                                              {
                                                                    //Need different name to differntiate between id, LOCAL id, DOUBLEDOTS id (??)
                                                                    //Maybe differnt AST_TAG (??)
                                                                    PrintParsing("lvalue","ID");                                                                    
                                                                    Object* obj = CreateObject("AST_TAG_LVALUE");
                                                                    obj->AddChild("$id", $1);
                                                                    $$ = obj;
                                                                }
                | LOCAL id                                      {
                                                                    PrintParsing("lvalue","local ID");
                                                                    Object* obj = CreateObject("AST_TAG_LVALUE");
                                                                    obj->AddChild("$localid", $2);
                                                                    $$ = obj;
                                                                }
                | DOUBLEDOTS id                                 {
                                                                    PrintParsing("lvalue","DOUBLEDOTS ID");
                                                                    Object* obj = CreateObject("AST_TAG_LVALUE");
                                                                    obj->AddChild("$doubledotsid", $2);
                                                                    $$ = obj;
                                                                }
                | member                                        {
                                                                    PrintParsing("lvalue","member");
                                                                    Object* obj = CreateObject("AST_TAG_LVALUE");
                                                                    obj->AddChild("$member", $1);
                                                                    $$ = obj;
                                                                }
                ;

member:         lvalue '.' id                                   {PrintParsing("member","lvalue . ID");}
                | lvalue '[' expr ']'                           {PrintParsing("member","lvalue [ expr ]");}
                | call '.' id                                   {PrintParsing("member","call . ID");}
                | call '[' expr ']'                             {PrintParsing("member","call [ expr ]");}
                ;

call:           call '(' elist ')'                              {PrintParsing("call","call ( elist )");}
                | lvalue callsuffix                             {PrintParsing("call","lvalue callsuffix");}
                | '(' funcdef ')' '(' elist ')'                 {PrintParsing("call","( funcdef ) ( elist )");}
                ;

callsuffix:     normcall                                        {PrintParsing("callsuffix","normcall");}
                | methodcall                                    {PrintParsing("callsuffix","methodcall");}
                ;

normcall:       '(' elist ')'                                   {PrintParsing("normcall","( elist )");}
                ;

methodcall:     DOUBLEDOTS id '(' elist ')'                     {PrintParsing("methodcall",":: ID ( elist )");}
                ;

elist:          elist ',' expr                                  {PrintParsing("elist","elist , expr");}
                |expr                                           {PrintParsing("elist","expr");}
                | /*empty*/                                     {PrintParsing("elist"," ");}
                ;

objectdef:      '['elist']'                                     {PrintParsing("objectdef","[ elist ]");}
                |'['indexed']'                                  {PrintParsing("indexed","indexed , indexedelem");}
                ;

indexed:        indexed ',' indexedelem                         {PrintParsing("indexed","indexed , indexedelem");}
                | indexedelem                                   {PrintParsing("indexed","indexedelem");}
                ;

indexedelem:    '{' expr ':' expr '}'                           {PrintParsing("indexedelem","{ expr : expr }");}
                ;

stmts:          stmts stmt                                      {PrintParsing("stmts","stmts stmt");}
                |                                               {PrintParsing("stmts", "empty");}
                ;

block:          '{' stmts '}'                                   {
                                                                    PrintParsing("block", "stmts");
                                                                    Object* obj = CreateObject(AST_TAG_BLOCK);
                                                                    obj->AddChild(std::string("$stmts"), $2);
                                                                    $$ = obj;
                                                                }
                ;
id:             ID                                              {
                                                                    PrintParsing("id", "ID"); 
                                                                    Object* obj = CreateObject(AST_TAG_ID);
                                                                    //std::cout << *$1 << std::endl; <-- uncomment and check.
                                                                    obj->value = new Value(*$1);
                                                                    $$ = obj;
                                                                }
funcdef:        FUNCTION '('  idlist ')' block                  {
                                                                    PrintParsing("funcdef", "FUNCTION (idlist) block");
                                                                }
                | FUNCTION id '(' idlist ')' block              {
                                                                    PrintParsing("funcdef", "FUNCTION ID (idlist) block");
                                                                    Object* obj = CreateObject(AST_TAG_FUNCDEF);
                                                                    
                                                                    obj->AddChild(std::string("$id"), $2);
                                                                    obj->AddChild(std::string("$idlist"), $4);
                                                                    obj->AddChild(std::string("$block"), $6);
                                                                    // TODO: Create value. Not sure what this should be
                                                                    // Maybe idlist and block children of value object.
                                                                    $$ = obj;
                                                                }
                ;

const:          NUMBER                                          {
                                                                    PrintParsing("const", "number");
                                                                    Object* obj = CreateObject(AST_TAG_CONST);
                                                                    obj->value = new Value($1);
                                                                    $$ = obj;
                                                                }
                | STRING                                        {
                                                                    PrintParsing("const", "STRING");
                                                                    Object* obj = CreateObject(AST_TAG_CONST);
                                                                    obj->value = new Value(*$1);
                                                                    $$ = obj;
                                                                }
                | NIL                                           {
                                                                    PrintParsing("const", "NIL");
                                                                    Object* obj = CreateObject(AST_TAG_CONST);
                                                                    obj->value = new Value(_NIL_);
                                                                    $$ = obj;
                                                                }
                | TRUE                                          {
                                                                    PrintParsing("const", "TRUE");
                                                                    Object* obj = CreateObject(AST_TAG_CONST);
                                                                    obj->value = new Value(true);
                                                                    $$ = obj;
                                                                }
                | FALSE                                         {
                                                                    PrintParsing("const", "FALSE");
                                                                    Object* obj = CreateObject(AST_TAG_CONST);
                                                                    obj->value = new Value(false);
                                                                    $$ = obj;
                                                                }
                ;

idlist:         idlist ',' id                                   {
                                                                    PrintParsing("idlist", "idlist , ID");
                                                                }
                |id                                             {
                                                                    PrintParsing("idlist", "ID");
                                                                }                                                                        
                |                                               {
                                                                    PrintParsing("idlist", "empty");
                                                                }
                ;


ifstmt:         IF '(' expr ')' stmt                            {PrintParsing("ifstmt", "IF ( expr ) stmt");}
                | IF '(' expr ')' stmt ELSE stmt                {PrintParsing("ifstmt", "IF ( expr ) stmt ELSE stmt");}
                ;

whilestmt:      WHILE '(' expr ')' stmt                         {
                                                                    PrintParsing("whilestmt", "WHILE ( expr ) stmt");
                                                                    //Two choices
                                                                    //1. Whilenode has 2 children, whilecond && whilestmt 
                                                                    Object* whileNode = CreateObject(AST_TAG_WHILE);
                                                                    whileNode->AddChild(std::string("$whilecond"), $3);
                                                                    whileNode->AddChild(std::string("$whilestmt"), $5);
                                                                    $$ = whileNode;
                                                                }
                ;

forstmt:        FOR '(' elist ';' expr ';' elist ')' stmt       {PrintParsing("forstmt", "FOR ( elist ; expr ; elist ) stmt");}
                ;

returnstmt:     RETURN ';'                                      {
                                                                    PrintParsing("returnstmt", " RETURN ;");
                                                                    Object* obj = CreateObject(AST_TAG_RETURNSTMT);
                                                                    //value of return is undef for now
                                                                    obj->value = new Value();
                                                                    $$ = obj;
                                                                }
                | RETURN expr ';'                               {
                                                                    PrintParsing("returnstmt", " RETURN expr ;");
                                                                    Object* obj = CreateObject(AST_TAG_RETURNSTMT);
                                                                    //made expr value of returnstmt for now
                                                                    obj->value = new Value(*$2);
                                                                    $$ = obj;
                                                                }
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