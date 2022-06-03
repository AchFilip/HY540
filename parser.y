%{
#include "./AST/Object.h"
#include "./AST/Value.h"
#include "./AST/TreeTags.h"
#include "./AST/PrintTreeVisitor.h"
#include "./AST/UnparseTreeVisitor.h"
#include "./AST/TreeHost.h"
#include "./AST/Intepreter.h"
#include <iostream>
#include <vector>


int yyerror(char const *s);
int yylex(void);

extern int yy_scan_string(const char *);
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

Value* ast;

// Prints the beginning and the end of a translated grammar rule
void PrintParsing(std::string s1, std::string s2){
    std::cout << s1 << " -> " << s2 << std::endl;
}

Value* CreateAstNodeOneChild(std::string nodeType, std::string childType, std::string disambiguate, const Value& child, double yylineno){
    Object* obj = new Object();
    const Value* keyVal = new Value(nodeType);
    const Value* lineVal = new Value(yylineno);
    obj->Set(AST_TAG_TYPE_KEY, *keyVal);
    obj->Set(AST_TAG_LINE_KEY, *lineVal);
    obj->Set(childType, child);
       if(!disambiguate.empty()){
        const Value* disambiguateVal = new Value(disambiguate);
        obj->Set(AST_TAG_DISAMBIGUATE_OBJECT, *disambiguateVal);
    }        
    return new Value(*obj);
}
Value* CreateAstNodeTwoChildren(std::string nodeType, std::string child1Type, std::string child2Type, std::string disambiguate, const Value& child1, const Value& child2, double yylineno){
    Object* obj = new Object();
    const Value* keyVal = new Value(nodeType);
    const Value* lineVal = new Value(yylineno);
    obj->Set(AST_TAG_TYPE_KEY, *keyVal);
    obj->Set(AST_TAG_LINE_KEY, *lineVal);
    if(!disambiguate.empty()){
        const Value* disambiguateVal = new Value(disambiguate);
        obj->Set(AST_TAG_DISAMBIGUATE_OBJECT, *disambiguateVal);
    } 
    obj->Set(child1Type, child1);
    obj->Set(child2Type, child2);
    return new Value(*obj);
}
%}


%start program

%code top {
    #include <iostream>
}

%code requires{
    #include "./AST/Value.h"
    #include "./AST/Object.h"
}

%union {
    std::string*    strVal;
    double          numVal;
    Value*          valVal;
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

%type <valVal> id const primary funcdef idlist block returnstmt expr whilestmt stmt stmts lvalue member call callsuffix objectdef assignexpr term ifstmt forstmt elist normcall methodcall indexed indexedelem;

/*  token rules */
%right '=' 
%left OR 
%left AND
%nonassoc EQEQ DIF DOUBLECOLON
%nonassoc '>' '<' BRANCHBIGEQ BRANCHSMALLEQ
%left '+' '-'
%left '*' '/' '%' 
%right NOT PP MM UMINUS
%left '.' DOUBLEDOTS 
%left '[' ']' 
%left '(' ')' 

%%
/*  grammar rules   */
program:        stmts                                                   {
                                                                            //PrintParsing("program","stmts");
                                                                            ast = $1;
                                                                        }
                ;

stmt:           expr ';'                                                {
                                                                            //PrintParsing("stmt","expr ;");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_STMT, AST_TAG_EXPR, "", *$1, yylineno);
                                                                        }
                | ifstmt                                                {
                                                                            //PrintParsing("stmt","ifstmt");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_STMT, AST_TAG_IF, "", *$1, yylineno);
                                                                        }  
                | whilestmt                                             {
                                                                            //PrintParsing("stmt","whiletmt");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_STMT, AST_TAG_WHILE, "", *$1, yylineno);
                                                                        }  
                | forstmt                                               {
                                                                            //PrintParsing("stmt","forstmt");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_STMT, AST_TAG_FOR, "", *$1, yylineno);
                                                                        }  
                | returnstmt                                            {
                                                                            //PrintParsing("stmt","returnstmt");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_STMT, AST_TAG_RETURNSTMT, "", *$1, yylineno);
                                                                        }  
                | BREAK ';'                                             {
                                                                            //PrintParsing("stmt","break ;");
                                                                            Object *breakObj = new Object();
                                                                            breakObj->Set(AST_TAG_TYPE_KEY, Value(std::string(AST_TAG_BREAK)));
                                                                            breakObj->Set(AST_TAG_LINE_KEY, Value((double)yylineno));
                                                                            Value *breakValue = new Value(*breakObj);
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_STMT, AST_TAG_BREAK, "", *breakValue, yylineno);
                                                                        }
                | CONTINUE ';'                                          {
                                                                            //PrintParsing("stmt","continue ;");                                                                            
                                                                            Object *continueObj = new Object();
                                                                            continueObj->Set(AST_TAG_TYPE_KEY, Value(std::string(AST_TAG_CONTINUE)));
                                                                            continueObj->Set(AST_TAG_LINE_KEY, Value((double)yylineno));
                                                                            Value *continueValue = new Value(*continueObj);
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_STMT, AST_TAG_CONTINUE, "", *continueValue, yylineno);
                                                                        }  
                | block                                                 {
                                                                            //PrintParsing("stmt","block");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_STMT, AST_TAG_BLOCK, "", *$1, yylineno);
                                                                        }  
                | funcdef                                               {
                                                                            //PrintParsing("stmt","funcdef");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_STMT, AST_TAG_FUNCDEF, "", *$1, yylineno);
                                                                        }  
                | ';'                                                   {
                                                                            //PrintParsing("stmt",";");
                                                                            $$ = new Value(_NIL_);
                                                                        }
                ;  

expr:           assignexpr                                              {
                                                                            //std::cout << "expr -> assignexpr" << std::endl;
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_EXPR, AST_TAG_ASSIGNEXPR, "", *$1, yylineno);
                                                                        }
                | expr '+' expr                                         {
                                                                            //PrintParsing("expr","expr + expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT, AST_TAG_EXPR_RIGHT, "+", *$1, *$3, yylineno);
                                                                        }
                | expr '-' expr                                         {
                                                                            //PrintParsing("expr","expr - expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, "-", *$1, *$3, yylineno);
                                                                        }
                | expr '*' expr                                         {
                                                                            //PrintParsing("expr","expr * expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, "*", *$1, *$3, yylineno);
                                                                            
                                                                        }
                | expr '/' expr                                         {
                                                                            //PrintParsing("expr","expr / expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, "/", *$1, *$3, yylineno);
                                                                            
                                                                        }
                | expr '%' expr                                         {
                                                                            //PrintParsing("expr","expr % expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, "%", *$1, *$3, yylineno);
                                                                            
                                                                        }
                | expr '>' expr                                         {
                                                                            //PrintParsing("expr","expr > expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, ">", *$1, *$3, yylineno);
                                                                            
                                                                        }
                | expr BRANCHBIGEQ expr                                 {
                                                                            //PrintParsing("expr","expr >= expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, ">=", *$1, *$3, yylineno);
                                                                            
                                                                        }
                | expr '<' expr                                         {
                                                                            //PrintParsing("expr","expr < expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, "<", *$1, *$3, yylineno);
                                                                            
                                                                        }
                | expr BRANCHSMALLEQ expr                               {
                                                                            //PrintParsing("expr","expr <= expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, "<=", *$1, *$3, yylineno);
                                                                            
                                                                        }
                | expr EQEQ expr                                        {
                                                                            //PrintParsing("expr","expr == expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, "==", *$1, *$3, yylineno);
                                                                            
                                                                        }
                | expr DIF expr                                         {
                                                                            //PrintParsing("expr","expr != expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, "!=", *$1, *$3, yylineno);
                                                                            
                                                                        }
                | expr AND expr                                         {
                                                                            //PrintParsing("expr","expr and expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, "&&", *$1, *$3, yylineno);
                                                                            
                                                                        }
                | expr OR expr                                          {
                                                                            //PrintParsing("expr","expr or expr");
                                                                            $$ = CreateAstNodeTwoChildren(AST_TAG_EXPR, AST_TAG_EXPR_LEFT,AST_TAG_EXPR_RIGHT, "||", *$1, *$3, yylineno);
                                                                            
                                                                        }
                | term                                                  {
                                                                            //PrintParsing("expr","term");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_EXPR, AST_TAG_TERM, "", *$1, yylineno);
                                                                        }
                ;

term:           '(' expr ')'                                            {
                                                                            //PrintParsing("term","( expr )");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, AST_TAG_EXPR, "(expr)", *$2, yylineno);                                                    
                                                                        }
                | '-' expr                                              {
                                                                            //PrintParsing("term","- expr");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, AST_TAG_EXPR, "-expr", *$2, yylineno);
                                                                        }
                | NOT lvalue                                            {
                                                                            //PrintParsing("term","not lvalue");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, AST_TAG_LVALUE, "notlvalue", *$2, yylineno);
                                                                        }
                | PP lvalue                                             {
                                                                            //PrintParsing("term","++ lvalue");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, AST_TAG_LVALUE, "++lvalue", *$2, yylineno);
                                                                        }
                | lvalue PP                                             {
                                                                            //PrintParsing("term","lvalue ++");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, AST_TAG_LVALUE, "lvalue++", *$1, yylineno);
                                                                        }
                | MM lvalue                                             {
                                                                            //PrintParsing("term","-- lvalue");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, AST_TAG_LVALUE, "--lvalue", *$2, yylineno);
                                                                        }
                | lvalue MM                                             {
                                                                            //PrintParsing("term","lvalue --");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, AST_TAG_LVALUE, "lvalue--", *$1, yylineno);
                                                                        }
                | primary                                               {
                                                                            //PrintParsing("term","primary");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, AST_TAG_PRIMARY, "", *$1, yylineno);
                                                                        }
                ;

assignexpr:     lvalue '=' expr                                 {
                                                                    //PrintParsing("assignexpr","lvalue = expr");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_ASSIGNEXPR, AST_TAG_LVALUE, AST_TAG_EXPR, "", *$1, *$3, yylineno);
                                                                }
                ;

primary:        lvalue                                          {
                                                                    //PrintParsing("primary","lvalue");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_PRIMARY, AST_TAG_LVALUE, "", *$1, yylineno);
                                                                }
                | call                                          {
                                                                    //PrintParsing("primary","call");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_PRIMARY, AST_TAG_CALL, "", *$1, yylineno);
                                                                }
                | objectdef                                     {
                                                                    //PrintParsing("primary","objectdef");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_PRIMARY, AST_TAG_OBJECTDEF, "", *$1, yylineno);
                                                                }
                | '(' funcdef ')'                               {
                                                                    //PrintParsing("primary","( funcdef )");;
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_PRIMARY, AST_TAG_FUNCDEF, "", *$2, yylineno);
                                                                }
                | const                                         {
                                                                    //PrintParsing("primary","const");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_PRIMARY, AST_TAG_CONST, "", *$1, yylineno);
                                                                }
                ;
                                        
lvalue:         id                                              {
                                                                    //PrintParsing("lvalue","ID");                                                                    
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_LVALUE, AST_TAG_ID, "id", *$1, yylineno);
                                                                }
                | LOCAL id                                      {
                                                                    //PrintParsing("lvalue","local ID");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_LVALUE, AST_TAG_ID, "local id", *$2, yylineno);
                                                                }
                | DOUBLECOLON id                                 {
                                                                    //PrintParsing("lvalue","DOUBLE_COLON ID");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_LVALUE, AST_TAG_ID, "doubledots id", *$2, yylineno);
                                                                }
                | member                                        {
                                                                    //PrintParsing("lvalue","member");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_LVALUE, AST_TAG_MEMBER, "member", *$1, yylineno);
                                                                }
                ;

member:         lvalue '.' id                                   {
                                                                    //PrintParsing("member","lvalue . ID");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_MEMBER, AST_TAG_LVALUE, AST_TAG_ID, ".id", *$1, *$3, yylineno);
                                                                }
                | lvalue '[' expr ']'                           {
                                                                    //PrintParsing("member","lvalue [ expr ]");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_MEMBER, AST_TAG_LVALUE, AST_TAG_EXPR, "[expr]", *$1, *$3, yylineno);
                                                                }
                | call '.' id                                   {
                                                                    //PrintParsing("member","call . ID");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_MEMBER, AST_TAG_CALL, AST_TAG_ID, ".id", *$1, *$3, yylineno);
                                                                }
                | call '[' expr ']'                             {
                                                                    //PrintParsing("member","call [ expr ]");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_MEMBER, AST_TAG_CALL, AST_TAG_EXPR, "[expr]", *$1, *$3, yylineno);
                                                                }
                ;

call:           call '(' elist ')'                              {
                                                                    //PrintParsing("call","call ( elist )");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_CALL, AST_TAG_CALL, AST_TAG_ELIST, "", *$1, *$3, yylineno);
                                                                }
                | lvalue callsuffix                             {
                                                                    //PrintParsing("call","lvalue callsuffix");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_CALL, AST_TAG_LVALUE, AST_TAG_CALLSUFFIX, "", *$1, *$2, yylineno);
                                                                }
                | '(' funcdef ')' '(' elist ')'                 {
                                                                    //PrintParsing("call","( funcdef ) ( elist )");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_CALL, AST_TAG_FUNCDEF, AST_TAG_ELIST, "", *$2, *$5, yylineno);
                                                                }
                ;

callsuffix:     normcall                                        {
                                                                    //PrintParsing("callsuffix","normcall");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_CALLSUFFIX, AST_TAG_NORMCALL, "", *$1, yylineno);
                                                                }
                | methodcall                                    {
                                                                    //PrintParsing("callsuffix","methodcall");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_CALLSUFFIX, AST_TAG_METHODCALL, "", *$1, yylineno);
                                                                }
                ;

normcall:       '(' elist ')'                                   {
                                                                    //PrintParsing("normcall","( elist )");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_NORMCALL, AST_TAG_ELIST, "", *$2, yylineno);
                                                                }
                ;

methodcall:     DOUBLEDOTS id '(' elist ')'                     {
                                                                    //PrintParsing("methodcall",":: ID ( elist )");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_METHODCALL, AST_TAG_ID, AST_TAG_ELIST, "", *$2, *$4, yylineno);
                                                                }
                ;

elist:          elist ',' expr                                  {
                                                                    //PrintParsing("elist","elist , expr");        
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_ELIST, AST_TAG_ELIST, AST_TAG_EXPR, "", *$1, *$3, yylineno);
                                                                }
                |expr                                           {
                                                                    //PrintParsing("elist","expr");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_ELIST, AST_TAG_EXPR, "", *$1, yylineno);
                                                                }
                | /*empty*/                                     {
                                                                    //PrintParsing("elist"," ");
                                                                    $$ = new Value(_NIL_);
                                                                }
                ;

objectdef:      '['elist']'                                     {
                                                                    //PrintParsing("objectdef","[ elist ]");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_OBJECTDEF, AST_TAG_ELIST, "", *$2, yylineno);
                                                                }
                |'['indexed']'                                  {
                                                                    //PrintParsing("objectdef","indexed");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_OBJECTDEF, AST_TAG_INDEXED, "", *$2, yylineno);
                                                                }
                ;

indexed:        indexed ',' indexedelem                         {
                                                                    //PrintParsing("indexed","indexed , indexedelem");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_INDEXED, AST_TAG_INDEXED, AST_TAG_INDEXEDELEM, "", *$1, *$3, yylineno);
                                                                }
                | indexedelem                                   {
                                                                    //PrintParsing("indexed","indexedelem");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_INDEXED, AST_TAG_INDEXEDELEM, "", *$1, yylineno);
                                                                }
                ;

indexedelem:    '{' expr ':' expr '}'                           {
                                                                    //PrintParsing("indexedelem","{ expr : expr }");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_INDEXEDELEM, AST_TAG_EXPR_LEFT, AST_TAG_EXPR_RIGHT, "", *$2, *$4, yylineno);
                                                                }
                ;

stmts:          stmts stmt                                      {
                                                                    //PrintParsing("stmts","stmts stmt");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_STMTS, AST_TAG_STMTS, AST_TAG_STMT, "", *$1, *$2, yylineno);
                                                                }
                |                                               {
                                                                    //PrintParsing("stmts", "empty");
                                                                    $$ = new Value(_NIL_);
                                                                }
                ;

block:          '{' stmts '}'                                   {
                                                                    //PrintParsing("block", "stmts");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_BLOCK, AST_TAG_STMTS, "", *$2, yylineno);
                                                                }
                ;
id:             ID                                              {
                                                                    //PrintParsing("id", "ID"); 
                                                                    $$ = new Value(*$1);
                                                                }
funcdef:        FUNCTION '('  idlist ')' block                  {
                                                                    //PrintParsing("funcdef", "FUNCTION (idlist) block");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_FUNCDEF, AST_TAG_IDLIST, AST_TAG_BLOCK, "", *$3, *$5, yylineno);
                                                                }
                | FUNCTION id '(' idlist ')' block              {
                                                                    //PrintParsing("funcdef", "FUNCTION ID (idlist) block");
                                                                    Object* obj = new Object();
                                                                    obj->Set(AST_TAG_TYPE_KEY, Value(std::string(AST_TAG_FUNCDEF)));
                                                                    obj->Set(AST_TAG_LINE_KEY, Value((double)yylineno));
                                                                    obj->Set(AST_TAG_ID, *$2);
                                                                    obj->Set(AST_TAG_IDLIST, *$4);
                                                                    obj->Set(AST_TAG_BLOCK, *$6);
                                                                    $$ = new Value(*obj);
                                                                }
                ;

const:          NUMBER                                          {
                                                                    //PrintParsing("const", "number");
                                                                    $$ = new Value($1);                                                                
                                                                }
                | STRING                                        {
                                                                    //PrintParsing("const", "STRING");
                                                                    $$ = new Value(*$1);
                                                                }
                | NIL                                           {
                                                                    //PrintParsing("const", "NIL");
                                                                    $$ = new Value(_NIL_);
                                                                }
                | TRUE                                          {
                                                                    //PrintParsing("const", "TRUE");
                                                                    $$ = new Value(true);
                                                                }
                | FALSE                                         {
                                                                    //PrintParsing("const", "FALSE");
                                                                    $$ = new Value(false);
                                                                }
                ;

idlist:         idlist ',' id                                   {
                                                                    //PrintParsing("idlist", "idlist , ID");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_IDLIST, AST_TAG_IDLIST, AST_TAG_ID, "", *$1, *$3, yylineno);
                                                                }
                |id                                             {
                                                                    //PrintParsing("idlist", "ID");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_IDLIST, AST_TAG_ID, "", *$1, yylineno);
                                                                }                                                                        
                |                                               {
                                                                    //PrintParsing("idlist", "empty");
                                                                    $$ = new Value(_NIL_);
                                                                }
                ;


ifstmt:         IF '(' expr ')' stmt                            {
                                                                    //PrintParsing("ifstmt", "IF ( expr ) stmt");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_IF, AST_TAG_EXPR, AST_TAG_IF_STMT, "", *$3, *$5, yylineno);
                                                                }
                | IF '(' expr ')' stmt ELSE stmt                {
                                                                    //PrintParsing("ifstmt", "IF ( expr ) stmt ELSE stmt");
                                                                    Object* obj = new Object();
                                                                    obj->Set(AST_TAG_TYPE_KEY, Value(std::string(AST_TAG_IF)));
                                                                    obj->Set(AST_TAG_LINE_KEY, Value((double)yylineno));
                                                                    obj->Set(AST_TAG_EXPR, *$3);
                                                                    obj->Set(AST_TAG_IF_STMT, *$5);
                                                                    obj->Set(AST_TAG_ELSE_STMT, *$7);
                                                                    $$ = new Value(*obj);
                                                                }
                ;

whilestmt:      WHILE '(' expr ')' stmt                         {
                                                                    //PrintParsing("whilestmt", "WHILE ( expr ) stmt");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_WHILE, AST_TAG_WHILE_COND, AST_TAG_WHILE_STMT, "", *$3, *$5, yylineno);
                                                                }
                ;
forstmt:        FOR '(' elist ';' expr ';' elist ')' stmt       {
                                                                    //PrintParsing("forstmt", "FOR ( elist ; expr ; elist ) stmt");
                                                                    Object* obj = new Object();
                                                                    obj->Set(AST_TAG_TYPE_KEY, Value(std::string(AST_TAG_FOR)));
                                                                    obj->Set(AST_TAG_LINE_KEY, Value((double)yylineno));
                                                                    obj->Set(AST_TAG_INIT, *$3);
                                                                    obj->Set(AST_TAG_EXPR, *$5);
                                                                    obj->Set(AST_TAG_FORCOND, *$7);
                                                                    obj->Set(AST_TAG_FORSTMT, *$9);
                                                                    $$ = new Value(*obj);
                                                                }
                ;

returnstmt:     RETURN ';'                                      {
                                                                    //PrintParsing("returnstmt", " RETURN ;");
                                                                    $$ = new Value();
                                                                }
                | RETURN expr ';'                               {
                                                                    //PrintParsing("returnstmt", " RETURN expr ;");     
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_RETURNSTMT, AST_TAG_EXPR, "", *$2, yylineno); 
                                                                }
                ;
                
%%

int yyerror(char const *s){
    std::cout << s << " in line: "<< yylineno << std::endl;
    exit(0);
}

class Parser{
public:
    Parser() = default;

    int Parse(std::string text){
        // Step 1: Create AST
        /* yyin = fopen(fileName.c_str(),"r"); */
        yy_scan_string(text.c_str());
        yyparse();

        //~~~~
        //Unparsing the code
        //TreeHost *treeHost = new TreeHost(); 
        //treeHost->Accept(new UnparseTreeVisitor(), *ast->ToObject()); 
        //std::cout << "AST: " << (*ast->ToObject())[UNPARSE_VALUE]->ToString() << std::endl;
        //std::ofstream MyFile("code.alpha");
        //MyFile << (*ast->ToObject())[UNPARSE_VALUE]->ToString();
        //MyFile.close();
        //End

        //~~~~~~
        // Test Interpreter functions

        Interpreter* interpreter = new Interpreter(false);
        interpreter->StartProgram(*ast->ToObject_NoConst());
        delete interpreter;

        //~~~~~~
        std::cout << "Its Over =)" << std::endl;
        return 0;
    }

private:

};