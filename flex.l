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

ID                      [A-Z]

WHITESPACE              [\t\r' ']

%%

{ID}            {std::cout<<"Found ID: " << yytext << std::endl;}
{KEYWORD}       {std::cout<<"Found Keyword: " << yytext << std::endl;}
{PUNCTUATION}   {std::cout<<"Found Punctuation: " << yytext << std::endl;}
{OPERATOR}      {std::cout<<"Found Operator: " << yytext << std::endl;}
{INTEGER_CONST} {std::cout<<"Found Integer: " << yytext << std::endl;}
{REAL_CONST}    {std::cout<<"Found Real: " << yytext << std::endl;}

"\n"            {}
{WHITESPACE}    {}
.               {std::cout<<"Can't recorgnise char :C " << yytext << std::endl;}

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
