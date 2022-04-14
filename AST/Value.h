// define AST tags for all grammar symbols you
// need to encompass in the syntax tree,
// all nodes have a 'type' and 'line' key
#pragma once

#include <cstring>
#include <string>
#include <tuple>

#include "TreeTags.h"

class Object;

enum NIL
{
    _NIL_ = 0
};
typedef void (*LibraryFunc)(Object &);
class Value
{
public:
    enum Type
    {
        UndefType = 0,
        NumberType,
        BooleanType,
        StringType,
        ObjectType,
        ProgramFunctionType,
        LibraryFunctionType,
        NativePtrType,
        NilType
    };
    using NativePtr = const std::tuple<void *, std::string>;

private:
    Type type;
    union
    {
        double numVal;
        bool boolVal;
        char* strVal;
        char* libFuncVal;
        const Object* objVal;
        struct
        {
            Object* ast;
            Object* closure;
        } progFuncVal;
        struct
        {
            void* ptr;
            char* typeId;
        } nativePtrVal;
    } data;

public:
    Value(void *ptr, const std::string &type);
    Value(const NativePtr &);
    Value(const std::string& strVal){
        type = StringType;
        char* temp = new char[strVal.length() + 1];
        strcpy(temp, strVal.c_str());
        data.strVal = temp;
    }
    Value(double number){
        type = NumberType;
        data.numVal = number;
    };
    Value(LibraryFunc);
    Value(enum NIL){
        type = NilType;
    };
    Value(bool boolVal){
        type = BooleanType;
        data.boolVal = boolVal;
    }
    Value(const Object & obj){
        type = ObjectType;
        data.objVal = &obj;
    };
    Value(const Value &);
    Value(void){
        type = UndefType;
    };
    ~Value();

public:
    Type GetType(void) const;
    double ToNumber(void) const;
    bool ToBool(void) const;
    const std::string &ToString(void) const;
    const Object *ToObject(void) const;
    const Object *ToProgramFunctionAST(void) const;
    const Object *ToProgramFunctionClosure(void) const;
    Object *ToObject_NoConst(void) const;
    Object *ToProgramFunctionAST_NoConst(void) const;
    Object *ToProgramFunctionClosure_NoConst(void) const;
    LibraryFunc ToLibraryFunction(void) const;
    void *ToNativePtr(void) const;
    const char *ToNativeTypeId(void) const;
    void FromNil(void);
    void FromString(const std::string &);
    void FromNativePtr(void *ptr, const std::string &type);
    void FromNativePtr(const NativePtr &); // alt style
    operator bool() const;                 // true test
    Value *Clone(void) const;
    const Value operator=(const Value &);

    // - here helper 'bool Is<type>(void)' const methods
    // - here similar 'From<type>(<type>)' methods
};