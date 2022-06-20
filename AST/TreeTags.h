#pragma once

#define AST_TAG_TYPE_KEY "$type"
#define AST_TAG_LINE_KEY "$line"
#define AST_TAG_DISAMBIGUATE_OBJECT "$disambiguate"

#define AST_TAG_VAR "$var"
#define AST_TAG_ID "$id"

#define AST_TAG std::string
#define AST_TAG_PROGRAM "$program"
#define AST_TAG_STMTS "$stmts"
#define AST_TAG_STMT "$stmt"

#define AST_TAG_EXPR "$expr"
#define AST_TAG_EXPR_LEFT "$expr_left"
#define AST_TAG_EXPR_RIGHT "$expr_right"

#define AST_TAG_TERM "$term"
#define AST_TAG_ASSIGNEXPR "$assignexpr"
#define AST_TAG_PRIMARY "$primary"
#define AST_TAG_LVALUE "$lvalue"
#define AST_TAG_MEMBER "$member"
#define AST_TAG_CALL "$call"
#define AST_TAG_CALLSUFFIX "$callsuffix"
#define AST_TAG_NORMCALL "$normcall"
#define AST_TAG_METHODCALL "$methodcall"
#define AST_TAG_ELIST "$elist"
#define AST_TAG_OBJECTDEF "$objectdef"
#define AST_TAG_INDEXED "$indexed"
#define AST_TAG_INDEXEDELEM "$indexed_elem"
#define AST_TAG_BLOCK "$block"
#define AST_TAG_FUNCDEF "$funcdef"
#define AST_TAG_ARGUMENT_LIST "$argumentList"
#define AST_TAG_CONST "$const"
#define AST_TAG_NUMBER "$number"
#define AST_TAG_IDLIST "$idlist"

#define AST_TAG_IF "$if"
#define AST_TAG_IF_STMT "$if_stmt"
#define AST_TAG_ELSE_STMT "$else_stmt"

#define AST_TAG_WHILE "$while"
#define AST_TAG_WHILE_COND "$while_cond"
#define AST_TAG_WHILE_STMT "$while_stmt"

#define AST_TAG_FOR "$for"
#define AST_TAG_INIT "$for_init"
#define AST_TAG_FORSTMT "$for_stmt"
#define AST_TAG_FORCOND "$for_cond"

#define AST_TAG_RETURNSTMT "$returnstmt"
#define AST_TAG_BREAK "$break"
#define AST_TAG_CONTINUE "$continue"

#define AST_TAG_QUASIQUOTES "$quasi_quotes"
#define AST_TAG_ESCAPE "$escape"

#define PARENT_FIELD "$parent"
#define OUTER_SCOPE_KEY "$outer"
#define PREVIOUS_SCOPE_KEY "$previous"
#define LOCAL_SCOPE_KEY "$local"
#define TAIL_SCOPE_KEY "$tail"
#define CLOSURE_SCOPE_KEY "$closure"
#define GLOBAL_SCOPE_KEY "$global"

#define RETVAL_RESERVED_FIELD "$retval"
#define RETVAL_SET(_v)                                                \
    if (true)                                                         \
        GetCurrentScopeSpace().Set(RETVAL_RESERVED_FIELD, Value(_v)); \
    else
#define RETVAL_GET() (GetCurrentScopeSpace()[RETVAL_RESERVED_FIELD] != nullptr) ? (*(GetCurrentScopeSpace()[RETVAL_RESERVED_FIELD])) : Value()