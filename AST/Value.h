// define AST tags for all grammar symbols you
// need to encompass in the syntax tree,
// all nodes have a 'type' and 'line' key
#pragma once

#define AST_TAG_TYPE_KEY    "type"
#define AST_TAG_LINE_KEY    "line"
#define AST_TAG_VAR         "var"
#define AST_TAG_LVALUE      "lvalue"
#define AST_TAG_ID          "id"
#define AST_TAG_STMT        "stmt"

#include <tuple>
#include <string>

class Object;

enum NIL{_NIL_ = 0};
typedef void (*LibraryFunc) (Object&);
class Value{
    public:
        enum Type{
            UndefType =  0,
            NumberType,
            BooleanType,
            StringType,
            ObjectType,
            ProgramFunctionType,
            LibraryFunctionType,
            NativePtrType,
            NilTypem
        };
        using NativePtr = const std::tuple<void*, std::string>;
    private:
        Type type;
        union{
            double  numVal;
            bool    boolVal;
            char*   strVal;
            char*   libFuncVal;
            struct {
                Object* ast;
                Object* closure;
            } progFuncVal;
            struct{
                void*   ptr;
                char*   typeId;
            } nativePtrVal;
        } data;
    public:
        Value(void* ptr, const std::string& type);
        Value(const NativePtr&);
        Value(const std::string&);
        Value(double);
        Value(LibraryFunc);
        Value(enum NIL);
        Value(const Object&);
        Value(const Value&);
        Value(void);
        ~Value();
    public:
        Type                GetType(void) const;
        double              ToNumber(void) const;
        bool                ToBool(void) const;
        const std::string&  ToString(void) const;
        const Object*	    ToObject (void) const;
        const Object*	    ToProgramFunctionAST (void) const;
        const Object*	    ToProgramFunctionClosure (void) const;
        Object*		        ToObject_NoConst (void) const;
        Object*		        ToProgramFunctionAST_NoConst (void) const;
        Object*		        ToProgramFunctionClosure_NoConst  (void) const;
        LibraryFunc	        ToLibraryFunction (void) const;
        void*		        ToNativePtr (void) const;
        const char*	        ToNativeTypeId (void) const;
        void		        FromNil (void);
        void		        FromString (const std::string&);
        void		        FromNativePtr  (void* ptr, const std::string& type);
        void		        FromNativePtr  (const NativePtr&); // alt style
        operator		    bool() const; // true test
        Value* 		        Clone (void) const;
        const Value 	    operator=(const Value&);

        // - here helper 'bool Is<type>(void)' const methods
        // - here similar 'From<type>(<type>)' methods

}; 