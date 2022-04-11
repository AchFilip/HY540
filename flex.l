%top{
#include "./AST/Object.h"
#include "./AST/Value.h"
#include "parser.hpp"
#include <iostream>
#include <vector>

struct aplha_token_t{
    int numLine;
    int numToken;
    std::string value;
    std::string type;
    std::string content;   
};

#define YY_DECL int yylex(void)

int nested_comments = 0;

// Replaces words in a string
// Source: https://www.quora.com/How-do-I-replace-all-occurrences-of-a-string-in-C
void findAndReplaceAll( std::string& data, const std::string& match, const std::string& replace){ 
    // Get the first occurrence 
    size_t pos = data.find(match); 
 
    // Repeat till end is reached 
    while( pos != std::string::npos) 
    { 
        data.replace(pos, match.size(), replace); 
      
        //Get the next occurrence from the current position 
        pos = data.find(match, pos+replace.size()); 
    } 
}

// Prints error message with bold red color
void printError(std::string s){
    std::cout<<"\033[1;31mCan't recorgnise char: " << s << "\033[0m" << std::endl;
}

}

%option noyywrap
%option yylineno


IF                      "if"
ELSE                    "else"
WHILE                   "while"
FOR                     "for"
FUNCTION                "function"
RETURN                  "return"
BREAK                   "break"
CONTINUE                "continue"
AND                     "and"
NOT                     "not"
OR                      "or"
LOCAL                   "local"
TRUE                    "true"
FALSE                   "false"
NIL                     "nil"

LEFT_CURLY_BRACKET      "{"
RIGHT_CURLY_BRACKET     "}"
LEFT_BRACKET            "["
RIGHT_BRACKET           "]"
LEFT_PARENTHESIS        "("
RIGHT_PARENTHESIS       ")"
SEMICOLON               ";"
COMMA                   ","
COLON                   ":"
DOUBLE_COLON            "::"
DOT                     "."
DOUBLE_DOT              ".."

ASSIGN                  "="
PLUS                    "+"
MINUS                   "-"
MULTIPLY                "*"
DIVISION                "/"
MOD                     "%"
INCREMENT               "++"
DECREMENT               "--"
EQUAL                   "=="
NOT_EQUAL               "!="
GREATER                 ">"
GREATER_EQUAL           ">="
LESS                    "<"
LESS_EQUAL              "<="

DIGIT                   [0-9]+
INTEGER_CONST           {DIGIT}
REAL_CONST              {INTEGER_CONST}{DOT}{INTEGER_CONST}
NUMBER                  {INTEGER_CONST}|{REAL_CONST}

ID                      [a-zA-Z][a-zA-Z_0-9]*

STRING                  \"(([^\"]|(\\\"))*(\n)*([^\"]|(\\\")))*\"

INLINECOMMENT           "//".*
COMMENT_START           "/*"
COMMENT_END             "*/"
WHITESPACE              [\t\r' '\n]

/*Rule(condition) for nested comments support*/
%x COMMENT

%%

{IF}                        {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return IF;}
{ELSE}                      {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return ELSE;}
{FOR}                       {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return FOR;}
{WHILE}                     {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return WHILE;}
{FUNCTION}                  {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return FUNCTION;}
{RETURN}                    {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return RETURN;}
{BREAK}                     {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return BREAK;}
{CONTINUE}                  {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return CONTINUE;}
{AND}                       {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return AND;}
{NOT}                       {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return NOT;}
{OR}                        {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return OR;}
{LOCAL}                     {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return LOCAL;}
{TRUE}                      {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return TRUE;}
{FALSE}                     {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return FALSE;}
{NIL}                       {std::cout << "Line: " << yylineno << " Found Keyword: " << yytext << std::endl; return NIL;}

{LEFT_CURLY_BRACKET}        {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return '{';}
{RIGHT_CURLY_BRACKET}       {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return '}';}
{LEFT_PARENTHESIS}          {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return '(';}
{RIGHT_PARENTHESIS}         {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return ')';}
{LEFT_BRACKET}              {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return '[';}
{RIGHT_BRACKET}             {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return ']';}
{SEMICOLON}                 {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return ';';}
{COMMA}                     {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return ',';}
{DOUBLE_COLON}              {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return DOUBLECOLON;}
{COLON}                     {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return ':';}
{DOUBLE_DOT}                {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return DOUBLEDOTS;}
{DOT}                       {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl; return '.';}

{INCREMENT}             {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return PP;}
{DECREMENT}             {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return MM;}
{EQUAL}                 {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return EQEQ;}
{NOT_EQUAL}             {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return DIF;}
{GREATER_EQUAL}         {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return BRANCHBIGEQ;}
{LESS_EQUAL}            {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return BRANCHSMALLEQ;}
{ASSIGN}                {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return '=';}
{PLUS}                  {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return '+';}
{MINUS}                 {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return '-';}
{MULTIPLY}              {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return '*';}
{DIVISION}              {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return '/';}
{MOD}                   {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return '%';}
{GREATER}               {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return '>';}
{LESS}                  {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl; return '<';}

{INLINECOMMENT}             {std::cout<<"Line: "<< yylineno << " Found InLineComment: " << yytext << std::endl;}
{STRING}                    {
                                std::string s = yytext;
                                if(s[s.length() - 2] == '\\')
                                {
                                    printError(s);
                                    exit(0);
                                }
                                findAndReplaceAll(s,"\\n","\n");
                                findAndReplaceAll(s,"\\t","\t");
                                findAndReplaceAll(s,"\\\"","\"");
                                findAndReplaceAll(s,"\\\\","\\");
                                std::cout<<"Line: "<< yylineno << " Fround String: " << s << std::endl;
                                    
                                // TODO: Copy to new string before adding to yyval;


                                return STRING;
                            }
{COMMENT_START}             {
                                std::cout<<"Line: "<< yylineno << " Found Comment start: " << yytext << std::endl;
                                BEGIN(COMMENT);
                            }
<COMMENT>{COMMENT_START}    {
                                ++nested_comments;
                                std::cout << "Line: "<< yylineno << " Nested comments: " << nested_comments << std::endl;
                            }
<COMMENT>"*"+               {}
<COMMENT>[^/*\n]+           {} 
<COMMENT>[/]                {} 
<COMMENT>\n                 {} 
<COMMENT><<EOF>>            {   
                                BEGIN(INITIAL);        
                                std::cout << "Comment doesn't end" << std::endl;
                            }  
<COMMENT>{COMMENT_END}      {
                                std::cout << "Line: "<< yylineno << " Found comment end: "<< yytext << std::endl;
                                if(nested_comments) --nested_comments;
                                else BEGIN(INITIAL);
                            }

{COMMENT_END}               {std::cout<<"Found Comment end - ERROR: has no starting comment: " << yytext << std::endl;}

{ID}                        {std::cout<<"Line: "<< yylineno << " Found ID: " << yytext << std::endl; return ID;}
{NUMBER}                    {std::cout<<"Line: "<< yylineno << " Found : " << yytext << std::endl; return NUMBER;}

{WHITESPACE}                {}
.                           {std::string s = yytext; printError(s);}

%%

