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
program:        stmts                                                   {PrintParsing("program","stmts");}
                ;

stmt:           expr ';'                                                {PrintParsing("stmt","expr");}
                | ifstmt                                                {PrintParsing("stmt","ifstmt");}  
                | whilestmt                                             {PrintParsing("stmt","whiletmt");}  
                | forstmt                                               {PrintParsing("stmt","forstmt");}  
                | returnstmt                                            {PrintParsing("stmt","returnstmt");}  
                | BREAK ';'                                             {PrintParsing("stmt","break ;");}  
                | CONTINUE ';'                                          {PrintParsing("stmt","continue ;");}  
                | block                                                 {PrintParsing("stmt","block");}  
                | funcdef                                               {PrintParsing("stmt","funcdef");}  
                | ';'                                                   {PrintParsing("stmt",";");}  

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

primary:        lvalue                                          {PrintParsing("primary","lvalue");}
                | call                                          {PrintParsing("primary","call");}
                | objectdef                                     {PrintParsing("primary","objectdef");}
                | '(' funcdef ')'                               {PrintParsing("primary","( funcdef )");}
                | const                                         {PrintParsing("primary","const");}
                ;

lvalue:         ID                                              {PrintParsing("lvalue","ID");}
                | LOCAL ID                                      {PrintParsing("lvalue","local ID");}
                | DOUBLEDOTS ID                                 {PrintParsing("lvalue","DOUBLEDOTS ID");}
                | member                                        {PrintParsing("lvalue","member");}
                ;

member:         lvalue '.' ID                                   {PrintParsing("member","lvalue . ID");}
                | lvalue '[' expr ']'                           {PrintParsing("member","lvalue [ expr ]");}
                | call '.' ID                                   {PrintParsing("member","call . ID");}
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

methodcall:     DOUBLEDOTS ID '(' elist ')'                     {PrintParsing("methodcall",":: ID ( elist )");}
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


