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

Object* ast;

// Prints the beginning and the end of a translated grammar rule
void PrintParsing(std::string s1, std::string s2){
    std::cout << s1 << " -> " << s2 << std::endl;
}

Object* CreateObject(AST_TAG ast_tag){
    Object* obj = new Object();
    obj->ast_tag = ast_tag;
    return obj;
}
Object* CreateExpressionNodeOperational(std::string ast_tag, std::string op, Object* child1, Object* child2){
    Object* exprObj = CreateObject(AST_TAG_EXPR);
    
    Object* operationalObj = CreateObject(ast_tag);
    operationalObj->value = new Value(op);
    operationalObj->AddChild("$child1", child1);
    operationalObj->AddChild("$child2", child2);

    exprObj->AddChild("$child", exprObj);
    return exprObj;
}
Object* CreateAstNodeOneChild(std::string ast_tag, std::string value, Object* child){
    Object* obj = CreateObject(ast_tag);
    obj->value = new Value(value);
    obj->AddChild("$child", child);
    return obj;
}
Object* CreateAstNodeTwoChildren(std::string ast_tag, std::string value, Object* child1, Object* child2){
    Object* obj = CreateObject(ast_tag);
    obj->value = new Value(value);
    obj->AddChild("$child1", child1);
    obj->AddChild("$child2", child2);
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

%type <objVal> id const primary funcdef idlist block returnstmt expr whilestmt stmt stmts lvalue member call callsuffix objectdef assignexpr term ifstmt forstmt elist normcall methodcall indexed indexedelem;

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
program:        stmts                                                   {
                                                                            PrintParsing("program","stmts");
                                                                            ast = $1;
                                                                        }
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
                                                                            Object* obj = CreateObject(AST_TAG_EXPR);
                                                                            obj->AddChild("$child", $1);
                                                                            $$ = obj;
                                                                        }
                | expr '+' expr                                         {
                                                                            PrintParsing("expr","expr + expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_ARITHMETIC, "+", $1, $3);
                                                                        }
                | expr '-' expr                                         {
                                                                            PrintParsing("expr","expr - expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_ARITHMETIC, "-", $1, $3);
                                                                        }
                | expr '*' expr                                         {
                                                                            PrintParsing("expr","expr * expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_ARITHMETIC, "*", $1, $3);
                                                                            
                                                                        }
                | expr '/' expr                                         {
                                                                            PrintParsing("expr","expr / expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_ARITHMETIC, "/", $1, $3);
                                                                            
                                                                        }
                | expr '%' expr                                         {
                                                                            PrintParsing("expr","expr % expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_ARITHMETIC, "%", $1, $3);
                                                                            
                                                                        }
                | expr '>' expr                                         {
                                                                            PrintParsing("expr","expr > expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_RELATIONAL, ">", $1, $3);
                                                                            
                                                                        }
                | expr BRANCHBIGEQ expr                                 {
                                                                            PrintParsing("expr","expr >= expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_RELATIONAL, ">=", $1, $3);
                                                                            
                                                                        }
                | expr '<' expr                                         {
                                                                            PrintParsing("expr","expr < expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_RELATIONAL, "<", $1, $3);
                                                                            
                                                                        }
                | expr BRANCHSMALLEQ expr                               {
                                                                            PrintParsing("expr","expr <= expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_RELATIONAL, "<=", $1, $3);
                                                                            
                                                                        }
                | expr EQEQ expr                                        {
                                                                            PrintParsing("expr","expr == expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_RELATIONAL, "==", $1, $3);
                                                                            
                                                                        }
                | expr DIF expr                                         {
                                                                            PrintParsing("expr","expr != expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_RELATIONAL, "!=", $1, $3);
                                                                            
                                                                        }
                | expr AND expr                                         {
                                                                            PrintParsing("expr","expr and expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_RELATIONAL, "&&", $1, $3);
                                                                            
                                                                        }
                | expr OR expr                                          {
                                                                            PrintParsing("expr","expr or expr");
                                                                            $$ = CreateExpressionNodeOperational(AST_TAG_EXPR_RELATIONAL, "||", $1, $3);
                                                                            
                                                                        }
                | term                                                  {
                                                                            PrintParsing("expr","term");
                                                                            Object* obj = CreateObject(AST_TAG_EXPR);
                                                                            obj->AddChild("$child", $1);
                                                                            $$ = obj;
                                                                        }
                ;

term:           '(' expr ')'                                            {
                                                                            PrintParsing("term","( expr )");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, "(expr)", $2);
                                                                        }
                | '-' expr                                              {
                                                                            PrintParsing("term","- expr");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, "-expr", $2);
                                                                        }
                | NOT lvalue                                            {
                                                                            PrintParsing("term","not lvalue");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, "notlvalue", $2);
                                                                        }
                | PP lvalue                                             {
                                                                            PrintParsing("term","++ lvalue");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, "++lvalue", $2);
                                                                        }
                | lvalue PP                                             {
                                                                            PrintParsing("term","lvalue ++");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, "lvalue++", $1);
                                                                        }
                | MM lvalue                                             {
                                                                            PrintParsing("term","-- lvalue");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, "--lvalue", $2);
                                                                        }
                | lvalue MM                                             {
                                                                            PrintParsing("term","lvalue --");
                                                                            $$ = CreateAstNodeOneChild(AST_TAG_TERM, "lvalue--", $1);
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
                                                                    PrintParsing("lvalue","ID");                                                                    
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_LVALUE, "id", $1);
                                                                }
                | LOCAL id                                      {
                                                                    PrintParsing("lvalue","local ID");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_LVALUE, "local_id", $2);
                                                                }
                | DOUBLEDOTS id                                 {
                                                                    PrintParsing("lvalue","DOUBLEDOTS ID");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_LVALUE, "doubledots_id", $2);
                                                                }
                | member                                        {
                                                                    PrintParsing("lvalue","member");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_LVALUE, "member", $1);
                                                                }
                ;

member:         lvalue '.' id                                   {
                                                                    PrintParsing("member","lvalue . ID");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_MEMBER, "lvalue.id", $1, $3);
                                                                }
                | lvalue '[' expr ']'                           {
                                                                    PrintParsing("member","lvalue [ expr ]");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_MEMBER, "lvalue[expr]", $1, $3);
                                                                }
                | call '.' id                                   {
                                                                    PrintParsing("member","call . ID");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_MEMBER, "call.id", $1, $3);
                                                                }
                | call '[' expr ']'                             {
                                                                    PrintParsing("member","call [ expr ]");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_MEMBER, "call[expr]", $1, $3);
                                                                }
                ;

call:           call '(' elist ')'                              {
                                                                    PrintParsing("call","call ( elist )");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_CALL, "call(elist)", $1, $3);
                                                                }
                | lvalue callsuffix                             {
                                                                    PrintParsing("call","lvalue callsuffix");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_CALL, "lvalue callsuffix", $1, $2);
                                                                }
                | '(' funcdef ')' '(' elist ')'                 {
                                                                    PrintParsing("call","( funcdef ) ( elist )");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_CALL, "(funcdef)(elist)", $2, $5);
                                                                }
                ;

callsuffix:     normcall                                        {
                                                                    PrintParsing("callsuffix","normcall");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_CALLSUFFIX, "normcall", $1);
                                                                }
                | methodcall                                    {
                                                                    PrintParsing("callsuffix","methodcall");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_CALLSUFFIX, "methodcall", $1);
                                                                }
                ;

normcall:       '(' elist ')'                                   {
                                                                    PrintParsing("normcall","( elist )");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_NORMCALL, "(elist)", $2);
                                                                }
                ;

methodcall:     DOUBLEDOTS id '(' elist ')'                     {
                                                                    PrintParsing("methodcall",":: ID ( elist )");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_METHODCALL, "::id(elist)", $2, $4);
                                                                }
                ;

elist:          elist ',' expr                                  {
                                                                    PrintParsing("elist","elist , expr");        
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_ELIST, "elist,expr", $1, $3);
                                                                }
                |expr                                           {
                                                                    PrintParsing("elist","expr");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_ELIST, "expr", $1);
                                                                }
                | /*empty*/                                     {
                                                                    PrintParsing("elist"," ");
                                                                    $$ = CreateObject(AST_TAG_ELIST);
                                                                }
                ;

objectdef:      '['elist']'                                     {
                                                                    PrintParsing("objectdef","[ elist ]");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_OBJECTDEF, "[elist]", $2);
                                                                }
                |'['indexed']'                                  {
                                                                    PrintParsing("indexed","indexed , indexedelem");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_OBJECTDEF, "[indexed]", $2);
                                                                }
                ;

indexed:        indexed ',' indexedelem                         {
                                                                    PrintParsing("indexed","indexed , indexedelem");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_INDEXED, "indexed,indexedelem", $1, $3);
                                                                }
                | indexedelem                                   {
                                                                    PrintParsing("indexed","indexedelem");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_INDEXED, "indexedelem", $1);
                                                                }
                ;

indexedelem:    '{' expr ':' expr '}'                           {
                                                                    PrintParsing("indexedelem","{ expr : expr }");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_INDEXEDELEM, "{expr:expr}", $2, $4);
                                                                }
                ;

stmts:          stmts stmt                                      {
                                                                    PrintParsing("stmts","stmts stmt");
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_STMTS, "stmts stmt", $1, $2);
                                                                }
                |                                               {
                                                                    PrintParsing("stmts", "empty");
                                                                    $$ = CreateObject(AST_TAG_STMTS);
                                                                }
                ;

block:          '{' stmts '}'                                   {
                                                                    PrintParsing("block", "stmts");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_BLOCK, "{stmts}", $2);
                                                                }
                ;
id:             ID                                              {
                                                                    PrintParsing("id", "ID"); 
                                                                    Object* obj = CreateObject(AST_TAG_ID);
                                                                    obj->value = new Value(*$1);
                                                                    $$ = obj;
                                                                }
funcdef:        FUNCTION '('  idlist ')' block                  {
                                                                    PrintParsing("funcdef", "FUNCTION (idlist) block");
                                                                    PrintParsing("funcdef", "FUNCTION ID (idlist) block");
                                                                    Object* obj = CreateObject(AST_TAG_FUNCDEF);
                                                                    obj->AddChild(std::string("$idlist"), $3);
                                                                    obj->AddChild(std::string("$block"), $5);
                                                                    // TODO: Create value. Not sure what this should be
                                                                    // Maybe idlist and block children of value object.
                                                                    $$ = obj;
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
                                                                    $$ = CreateAstNodeTwoChildren(AST_TAG_IDLIST, "idlist,id", $1, $3);
                                                                }
                |id                                             {
                                                                    PrintParsing("idlist", "ID");
                                                                    $$ = CreateAstNodeOneChild(AST_TAG_IDLIST, "id", $1);
                                                                }                                                                        
                |                                               {
                                                                    PrintParsing("idlist", "empty");
                                                                    $$ = CreateObject(AST_TAG_IDLIST);
                                                                }
                ;


ifstmt:         IF '(' expr ')' stmt                            {
                                                                    PrintParsing("ifstmt", "IF ( expr ) stmt");
                                                                    Object* ifNode = CreateObject(AST_TAG_IF);
                                                                    ifNode->AddChild(std::string("$ifcond"), $3);
                                                                    ifNode->AddChild(std::string("$ifstmt"), $5);
                                                                    $$ = ifNode;
                                                                }
                | IF '(' expr ')' stmt ELSE stmt                {
                                                                    PrintParsing("ifstmt", "IF ( expr ) stmt ELSE stmt");
                                                                    Object* ifNode = CreateObject(AST_TAG_IF);
                                                                    ifNode->AddChild(std::string("$ifcond"), $3);
                                                                    ifNode->AddChild(std::string("$ifstmt"), $5);
                                                                    ifNode->AddChild(std::string("$elsestmt"), $7);
                                                                    $$ = ifNode;
                                                                }
                ;

whilestmt:      WHILE '(' expr ')' stmt                         {
                                                                    PrintParsing("whilestmt", "WHILE ( expr ) stmt");
                                                                    Object* whileNode = CreateObject(AST_TAG_WHILE);
                                                                    whileNode->AddChild(std::string("$whilecond"), $3);
                                                                    whileNode->AddChild(std::string("$whilestmt"), $5);
                                                                    $$ = whileNode;
                                                                }
                ;

forstmt:        FOR '(' elist ';' expr ';' elist ')' stmt       {
                                                                    PrintParsing("forstmt", "FOR ( elist ; expr ; elist ) stmt");
                                                                    Object* forNode = CreateObject(AST_TAG_FOR);
                                                                    forNode->AddChild(std::string("$init"), $3);
                                                                    forNode->AddChild(std::string("$cond"), $5);
                                                                    forNode->AddChild(std::string("$expr"), $7);
                                                                    forNode->AddChild(std::string("$stmt"), $9);
                                                                    $$ = forNode;
                                                                }
                ;

returnstmt:     RETURN ';'                                      {
                                                                    PrintParsing("returnstmt", " RETURN ;");
                                                                    Object* obj = CreateObject(AST_TAG_RETURNSTMT);
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

    // Example of accesing the ast after parsing. ast is the root node.
    // std::cout << "Root children: " << ast->children.size() << std::endl;
    ast->RecursivePrint(0);
    Object::CreateGraph(ast);

    return 0;
}