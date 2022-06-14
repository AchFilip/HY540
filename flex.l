%top{
#include "./AST/Object.h"
#include "./AST/Value.h"
#include "parser.hpp"
#include <iostream>
#include <vector>
#include <string>

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
    std::cout<<"\033[1;31m Can't recorgnise char: " << s << "\033[0m" << std::endl;
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
AND                     "&&"
NOT                     "!"
OR                      "||"
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

PARSE_EXPR              "@;"
ID                      [$]?[a-zA-Z][a-zA-Z_0-9]*

STRING                  \"(([^\"]|(\\\"))*(\n)*([^\"]|(\\\")))*\"

INLINECOMMENT           "//".*
COMMENT_START           "/*"
COMMENT_END             "*/"
WHITESPACE              [\t\r' '\n]

/*Rule(condition) for nested comments support*/
%x COMMENT

%%

{IF}                        {return IF;}
{ELSE}                      {return ELSE;}
{FOR}                       {return FOR;}
{WHILE}                     {return WHILE;}
{FUNCTION}                  {return FUNCTION;}
{RETURN}                    {return RETURN;}
{BREAK}                     {return BREAK;}
{CONTINUE}                  {return CONTINUE;}
{AND}                       {return AND;}
{NOT}                       {return NOT;}
{OR}                        {return OR;}
{LOCAL}                     {return LOCAL;}
{TRUE}                      {return TRUE;}
{FALSE}                     {return FALSE;}
{NIL}                       {return NIL;}

{LEFT_CURLY_BRACKET}        {return '{';}
{RIGHT_CURLY_BRACKET}       {return '}';}
{LEFT_PARENTHESIS}          {return '(';}
{RIGHT_PARENTHESIS}         {return ')';}
{LEFT_BRACKET}              {return '[';}
{RIGHT_BRACKET}             {return ']';}
{SEMICOLON}                 {return ';';}
{COMMA}                     {return ',';}
{DOUBLE_COLON}              {return DOUBLECOLON;}
{COLON}                     {return ':';}
{DOUBLE_DOT}                {return DOUBLEDOTS;}
{DOT}                       {return '.';}

{INCREMENT}                 {return PP;}
{DECREMENT}                 {return MM;}
{EQUAL}                     {return EQEQ;}
{NOT_EQUAL}                 {return DIF;}
{GREATER_EQUAL}             {return BRANCHBIGEQ;}
{LESS_EQUAL}                {return BRANCHSMALLEQ;}
{ASSIGN}                    {return '=';}
{PLUS}                      {return '+';}
{MINUS}                     {return '-';}
{MULTIPLY}                  {return '*';}
{DIVISION}                  {return '/';}
{MOD}                       {return '%';}
{GREATER}                   {return '>';}
{LESS}                      {return '<';}

{INLINECOMMENT}             {}
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
                                s = s.substr(1, s.size() - 2);

                                //parsing it with ""    
                                yylval.strVal = new std::string(s);

                                return STRING;
                            }
{COMMENT_START}             {
                                BEGIN(COMMENT);
                            }
<COMMENT>{COMMENT_START}    {
                                ++nested_comments;
                            }
<COMMENT>"*"+               {}
<COMMENT>[^/*\n]+           {} 
<COMMENT>[/]                {} 
<COMMENT>\n                 {} 
<COMMENT><<EOF>>            {   
                                BEGIN(INITIAL);        
                            }  
<COMMENT>{COMMENT_END}      {
                                if(nested_comments) --nested_comments;
                                else BEGIN(INITIAL);
                            }

{COMMENT_END}               {}
                            
{ID}                        {std::string *s = new std::string(strdup(yytext)); /*<-- can be skipped*/ yylval.strVal = s; return ID;} 
{NUMBER}                    {yylval.numVal = std::stod(yytext); return NUMBER;}

{WHITESPACE}                {}
.                           {std::string s = yytext; printError(s);}

{PARSE_EXPR}                {
                                return PARSE_EXPR;
                            }

%%

