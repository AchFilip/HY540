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

%type <objVal> id const primary funcdef idlist block;

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

stmt:           expr ';'                                                {PrintParsing("stmt","expr ;");}
                | ifstmt                                                {PrintParsing("stmt","ifstmt");}  
                | whilestmt                                             {PrintParsing("stmt","whiletmt");}  
                | forstmt                                               {PrintParsing("stmt","forstmt");}  
                | returnstmt                                            {PrintParsing("stmt","returnstmt");}  
                | BREAK ';'                                             {PrintParsing("stmt","break ;");}  
                | CONTINUE ';'                                          {PrintParsing("stmt","continue ;");}  
                | block                                                 {PrintParsing("stmt","block");}  
                | funcdef                                               {PrintParsing("stmt","funcdef");}  
                | ';'                                                   {PrintParsing("stmt",";");}
                ;  

expr:           assignexpr                                              {std::cout << "expr -> assignexpr" << std::endl;}
                | expr '+' expr                                         {PrintParsing("expr","expr + expr");}
                | expr '-' expr                                         {PrintParsing("expr","expr - expr");}
                | expr '*' expr                                         {PrintParsing("expr","expr * expr");}
                | expr '/' expr                                         {PrintParsing("expr","expr / expr");}
                | expr '%' expr                                         {PrintParsing("expr","expr % expr");}
                | expr '>' expr                                         {PrintParsing("expr","expr > expr");}
                | expr BRANCHBIGEQ expr                                 {PrintParsing("expr","expr >= expr");}
                | expr '<' expr                                         {PrintParsing("expr","expr < expr");}
                | expr BRANCHSMALLEQ expr                               {PrintParsing("expr","expr <= expr");}
                | expr EQEQ expr                                        {PrintParsing("expr","expr == expr");}
                | expr DIF expr                                         {PrintParsing("expr","expr != expr");}
                | expr AND expr                                         {PrintParsing("expr","expr and expr");}
                | expr OR expr                                          {PrintParsing("expr","expr or expr");}
                | term                                                  {PrintParsing("expr","term");}
                ;

term:           '(' expr ')'                                            {PrintParsing("term","( expr )");}
                | '-' expr                                              {PrintParsing("term","- expr");}
                | NOT lvalue                                            {PrintParsing("term","not lvalue");}
                | PP lvalue                                             {PrintParsing("term","++ lvalue");}
                | lvalue PP                                             {PrintParsing("term","lvalue ++");}
                | MM lvalue                                             {PrintParsing("term","-- lvalue");}
                | lvalue MM                                             {PrintParsing("term","lvalue --");}
                | primary                                               {PrintParsing("term","primary");}
                ;

assignexpr:     lvalue '=' expr                                 {PrintParsing("assignexpr","lvalue = expr");}
                ;

primary:        lvalue                                          {
                                                                    PrintParsing("primary","lvalue");
                                                                }
                | call                                          {
                                                                    PrintParsing("primary","call");
                                                                }
                | objectdef                                     {
                                                                    PrintParsing("primary","objectdef");
                                                                }
                | '(' funcdef ')'                               {
                                                                    PrintParsing("primary","( funcdef )");
                                                                }
                | const                                         {
                                                                    PrintParsing("primary","const");
                                                                    Object* obj = CreateObject(AST_TAG_PRIMARY);
                                                                    obj->AddChild("$child", $1);
                                                                    $$ = obj;
                                                                }
                ;

lvalue:         id                                              {PrintParsing("lvalue","ID");}
                | LOCAL id                                      {PrintParsing("lvalue","local ID");}
                | DOUBLEDOTS id                                 {PrintParsing("lvalue","DOUBLEDOTS ID");}
                | member                                        {PrintParsing("lvalue","member");}
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

block:          '{' stmts '}'                                   {PrintParsing("block", "stmts");}
                ;
id:             ID                                              {
                                                                    Object* obj = CreateObject(AST_TAG_ID);
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

idlist:         idlist ',' id                                   {PrintParsing("idlist", "idlist , ID");}
                |id                                             {PrintParsing("idlist", "ID");}                                                                        
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