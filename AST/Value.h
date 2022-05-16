// define AST tags for all grammar symbols you
// need to encompass in the syntax tree,
// all nodes have a 'type' and 'line' key
#pragma once

#include <cstring>
#include <string>
#include <tuple>
#include <assert.h>

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
        char *strVal;
        char *libFuncVal;
        const Object *objVal;
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
    } data;

public:
    Value(void *ptr, const std::string &type);
    Value(const NativePtr &);
    Value(const std::string &strVal)
    {
        type = StringType;
        char *temp = new char[strVal.length() + 1];
        strcpy(temp, strVal.c_str());
        data.strVal = temp;
    }
    Value(double number)
    {
        type = NumberType;
        data.numVal = number;
    };
    Value(LibraryFunc);
    Value(enum NIL)
    {
        type = NilType;
    };
    Value(bool boolVal)
    {
        type = BooleanType;
        data.boolVal = boolVal;
    }
    Value(const Object &obj)
    {
        type = ObjectType;
        data.objVal = &obj;
    };
    // Value(const Value &);
    Value(void)
    {
        type = UndefType;
    };
    // ~Value();

    Type GetType(void) const
    {
        switch (type)
        {
        case UndefType:
            return UndefType;
            break;
        case NumberType:
            return NumberType;
            break;
        case BooleanType:
            return BooleanType;
            break;
        case StringType:
            return StringType;
            break;
        case ObjectType:
            return ObjectType;
            break;
        case ProgramFunctionType:
            return ProgramFunctionType;
            break;
        case LibraryFunctionType:
            return LibraryFunctionType;
            break;
        case NativePtrType:
            return NativePtrType;
            break;
        case NilType:
            return NilType;
            break;
        default:
            break;
        }
        return NilType;
    }
    std::string Stringify() const
    {
        if (this->GetType() == UndefType)
            return "Undef";
        else if (this->GetType() == NumberType)
            return std::to_string(data.numVal);
        else if (this->GetType() == BooleanType)
            return std::to_string(data.boolVal);
        else if (this->GetType() == StringType)
            return data.strVal;
        else if (this->GetType() == NilType)
            return "Nill";
        else if (this->GetType() == ObjectType)
            return "Object";
        else
            assert(false);
        return "";
    }
    double ToNumber(void) const
    {
        return data.numVal;
    }
    bool ToBool(void) const
    {
        return data.boolVal;
    }
    const std::string ToString(void) const
    {
        // if (this->data.strVal)
        return std::string(this->data.strVal);
        // else
        // {
        //     std::cout << "empty strVal" << std::endl;
        //     return "";
        // }
    };
    const Object *ToObject(void) const
    {
        return data.objVal;
    };
    const Object *ToProgramFunctionAST(void) const;
    const Object *ToProgramFunctionClosure(void) const;
    Object *ToObject_NoConst(void) const
    {
        return const_cast<Object *>(data.objVal);
    }
    Object *ToProgramFunctionAST_NoConst(void) const;
    Object *ToProgramFunctionClosure_NoConst(void) const;
    LibraryFunc ToLibraryFunction(void) const;
    void *ToNativePtr(void) const;
    const char *ToNativeTypeId(void) const;
    void FromNil(void);
    void FromString(const std::string &);
    void FromNativePtr(void *ptr, const std::string &type);
    void FromNativePtr(const NativePtr &); // alt style
    Value *Clone(void) const;

    // Cast overloading
    operator bool() const
    { // true test
        // TODO: Implement code for bool type checking
        switch (GetType())
        {
        case NilType:
        case UndefType:
            return false;
        case NumberType:
            return data.numVal != 0;
        case StringType:
            return data.strVal != nullptr; // Maybe not?!
        default:
            assert(false && "Not implemented yet");
        }
    }
    operator double() const
    {
        switch (GetType())
        {
        case NilType:
            return 0;
        case UndefType:
            return 0;
        case NumberType:
            return data.numVal;
        case StringType:
            return std::stod(data.strVal); // Maybe not?!
        default:
            assert(false && "Not implemented yet");
        }
    }
    operator std::string() const
    {
        switch (GetType())
        {
        case NilType:
            return "\0";
        case UndefType:
            return "\0";
        case NumberType:
            return std::to_string(data.numVal);
        case StringType:
            return data.strVal;
        default:
            assert(false && "Not implemented yet");
        }
    }

    // Operator overloading. Use overloaded casting operators to save repeatable code
    const Value operator=(const Value &right)
    {
        this->type = right.type;
        this->data = right.data; // NOTE: Maybe copy the value in case of pointers?
        return *this;
    };
    const Value operator+(const Value &right)
    {
        switch (right.GetType())
        {
        case NumberType:
            return (double)(*this) + (double)right;
        case StringType:
            return (std::string)(*this) + (std::string)right;
        default:
            // Maybe throw exception?
            assert(false && "Not implemented yet");
        }
    }
    const Value operator-(const Value &right)
    {
        switch (right.GetType())
        {
        case NumberType:
            return (double)(*this) - right.ToNumber();
        default:
            // Maybe throw exception?
            assert(false && "Not implemented yet");
        }
    }
    const Value operator*(const Value &right)
    {
        switch (right.GetType())
        {
        case NumberType:
            return (double)(*this) * right.ToNumber();
        default:
            // Maybe throw exception?
            assert(false && "Not implemented yet");
        }
    }
    const Value operator/(const Value &right)
    {
        switch (right.GetType())
        {
        case NumberType:
            return (double)(*this) / right.ToNumber();
        default:
            // Maybe throw exception?
            assert(false && "Not implemented yet");
        }
    }
    const Value operator%(const Value &right)
    {
        switch (right.GetType())
        {
        case NumberType:
            return (double)((int)(double)(*this) % (int)right.ToNumber());
        default:
            // Maybe throw exception?
            assert(false && "Not implemented yet");
        }
    }
    const Value operator==(const Value &right)
    {
        // TODO: should not allow different types in relational expr
        switch (right.GetType())
        {
        case NumberType:
            return (double)(*this) == right.ToNumber();
        case BooleanType:
            return (bool)(*this) == right.ToBool();
        case StringType:
            return (std::string)(*this) == right.ToString();
        default:
            // Maybe throw exception?
            assert(false && "Not implemented yet");
        }
    }
    const Value operator!=(const Value &right)
    {
        // TODO: should not allow different types in relational expr
        switch (right.GetType())
        {
        case NumberType:
            return (double)(*this) != right.ToNumber();
        case BooleanType:
            return (bool)(*this) != right.ToBool();
        case StringType:
            return (std::string)(*this) != right.ToString();
        default:
            // Maybe throw exception?
            assert(false && "Not implemented yet");
        }
    }
    const Value operator>(const Value &right)
    {
        if (right.GetType() != type)
        {
            // should not allow different types in relational expr
            assert(false && "Not implemented yet");
        }
        switch (right.GetType())
        {
        case NumberType:
            return (double)(*this) > right.ToNumber();
        case StringType:
            return (std::string)(*this) > right.ToString();
        default:
            // Maybe throw exception?
            assert(false && "Not implemented yet");
        }
    }
    const Value operator>=(const Value &right)
    {
        if (right.GetType() != type)
        {
            // should not allow different types in relational expr
            assert(false && "Not implemented yet");
        }
        switch (right.GetType())
        {
        case NumberType:
            return (double)(*this) >= right.ToNumber();
        case StringType:
            return (std::string)(*this) >= right.ToString();
        default:
            // Maybe throw exception?
            assert(false && "Not implemented yet");
        }
    }
    const Value operator<(const Value &right)
    {
        if (right.GetType() != type)
        {
            // should not allow different types in relational expr
            assert(false && "Not implemented yet");
        }
        switch (right.GetType())
        {
        case NumberType:
            return (double)(*this) < right.ToNumber();
        case StringType:
            return (std::string)(*this) < right.ToString();
        default:
            // Maybe throw exception?
            assert(false && "Not implemented yet");
        }
    }
    const Value operator<=(const Value &right)
    {
        if (right.GetType() != type)
        {
            // should not allow different types in relational expr
            assert(false && "Not implemented yet");
        }
        switch (right.GetType())
        {
        case NumberType:
            return (double)(*this) <= right.ToNumber();
        case StringType:
            return (std::string)(*this) <= right.ToString();
        default:
            // Maybe throw exception?
            assert(false && "Not implemented yet");
        }
    }
    const Value operator&&(const Value &right)
    {
        return this && right;
    }
    const Value operator||(const Value &right)
    {
        return this || right;
    }

    const Value operator++()
    {
        // should be number check and throw exception?? TODO:
        data.numVal += 1;
        return data.numVal;
    }
    const Value operator--()
    {
        // should be number check and throw exception?? TODO:
        return --data.numVal;
    }
    const Value operator-()
    {
        // should be number check and throw exception?? TODO:
        return data.numVal * -1;
    }
    const Value operator!()
    {
        // should be boll check and throw exception?? TODO:
        return !data.boolVal;
    }

    // - here helper 'bool Is<type>(void)' const methods
    // - here similar 'From<type>(<type>)' methods
};
