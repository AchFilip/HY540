%top{
#include <iostream>
#include <vector>

struct aplha_token_t{
    int numLine;
    int numToken;
    std::string value;
    std::string type;
    std::string content;   
};

#define YY_DECL int a_yylex(void* yylval)

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
KEYWORD                 {IF}|{ELSE}|{WHILE}|{FOR}|{FUNCTION}|{RETURN}|{BREAK}|{CONTINUE}|{AND}|{NOT}|{OR}|{LOCAL}|{TRUE}|{FALSE}|{NIL}

LEFT_CURLY_BRACKET      "{"
RIGHT_CURLY_BRACKET     "}"
CURLY_BRACKET           {LEFT_CURLY_BRACKET}|{RIGHT_CURLY_BRACKET}
LEFT_BRACKET            "["
RIGHT_BRACKET           "]"
BRACKET                 {LEFT_BRACKET}|{RIGHT_BRACKET}
LEFT_PARENTHESIS        "("
RIGHT_PARENTHESIS       ")"
PARENTHESIS             {LEFT_PARENTHESIS}|{RIGHT_PARENTHESIS}
SEMICOLON               ";"
COMMA                   ","
COLON                   ":"
DOUBLE_COLON            "::"
DOT                     "."
DOUBLE_DOT              ".."
PUNCTUATION             {CURLY_BRACKET}|{BRACKET}|{PARENTHESIS}|{SEMICOLON}|{COMMA}|{COLON}|{DOUBLE_COLON}|{DOT}|{DOUBLE_DOT}

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
OPERATOR                {ASSIGN}|{PLUS}|{MINUS}|{MULTIPLY}|{DIVISION}|{MOD}|{INCREMENT}|{DECREMENT}|{EQUAL}|{NOT_EQUAL}|{GREATER}|{GREATER_EQUAL}|{LESS}|{LESS_EQUAL}

DIGIT                   [0-9]
INTEGER_CONST           {DIGIT}
REAL_CONST              {INTEGER_CONST}{DOT}{INTEGER_CONST}

ID                      [a-zA-Z][a-zA-Z_0-9]*

STRING          \"(([^\"]|(\\\"))*(\n)*([^\"]|(\\\")))*\"

INLINECOMMENT           "//".*
COMMENT_START           "/*"
COMMENT_END             "*/"
WHITESPACE              [\t\r' '\n]

/*Rule(condition) for nested comments support*/
%x COMMENT

%%

{KEYWORD}                   {std::cout<<"Line: "<< yylineno << " Found Keyword: " << yytext << std::endl;}
{PUNCTUATION}               {std::cout<<"Line: "<< yylineno << " Found Punctuation: " << yytext << std::endl;}
{OPERATOR}                  {std::cout<<"Line: "<< yylineno << " Found Operator: " << yytext << std::endl;}
{INLINECOMMENT}             {std::cout<<"Line: "<< yylineno << " Found InLineComment: " << yytext << std::endl;}
{STRING}                    {
                                std::string s = yytext;
                                findAndReplaceAll(s,"\\n","\n");
                                findAndReplaceAll(s,"\\t","\t");
                                findAndReplaceAll(s,"\\\"","\"");
                                findAndReplaceAll(s,"\\\\","\\");
                                std::cout<<"Line: "<< yylineno << " Fround String: " << s << std::endl;
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

{ID}                        {std::cout<<"Line: "<< yylineno << " Found ID: " << yytext << std::endl;}
{INTEGER_CONST}             {std::cout<<"Line: "<< yylineno << " Found Integer: " << yytext << std::endl;}
{REAL_CONST}                {std::cout<<"Line: "<< yylineno << " Found Real: " << yytext << std::endl;}

{WHITESPACE}                {}
.                           {std::string s = yytext; printError(s);}

%%

int main(int argc, char** argv){
    std::vector<aplha_token_t>* myTokens = new std::vector<aplha_token_t>;
    if(argc>1){
        if(!(yyin=fopen(argv[1],"r"))){
            fprintf(stderr,"cannto access");
            return 1;
        }
    }else{
        yyin=stdin;
    }

    a_yylex(myTokens);
    return 0;
}
