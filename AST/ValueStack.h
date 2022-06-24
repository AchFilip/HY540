#pragma once
#include <list>
#include <assert.h>
#include "Value.h"

class ValueStack
{

protected:
	std::list<Value> valueStack;

public:
	bool IsEmpty(void) const
	{
		return valueStack.empty();
	}
	void Clear(void)
	{
		valueStack.clear();
	}
	void Push(const Value &v)
	{
		valueStack.push_front(v);
	}
	void Pop()
	{
		assert(!IsEmpty());
		valueStack.pop_front();
	}
	int Size(){
		return valueStack.size();
	}
	auto Top(void) const -> const Value &
	{
		assert(!IsEmpty());
		return valueStack.front();
	}
	auto GetTopAndPop(void) -> const Value
	{
		assert(!IsEmpty());
		auto v = valueStack.front();
		valueStack.pop_front();
		return v;
	}

	void Debug_Print(){
		std::cout << "Value Stack Print" << std::endl;
		for(auto it = valueStack.begin(); it != valueStack.end(); ++it){
			if(it->GetType() == Value::ObjectType)
				it->ToObject()->Debug_PrintChildren();
			else
				std::cout << "\t" << it->Stringify() << std::endl;
		}
	}

	ValueStack(void) = default;
	ValueStack(const ValueStack &) = delete;
	ValueStack(ValueStack &&) = delete;
};