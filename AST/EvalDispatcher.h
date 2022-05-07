#pragma once

#include <map>
#include "./Object.h"


class EvalDispatcher
{
public:
    using F = std::function<const Value(Object &)>;

private:
    std::map<std::string, F> evals;

	
public:
    void Install(const std::string &tag, const F &f)
    {
        evals[tag] = f;
    }
    const Value Eval(Object &node)
    {
        return evals[node[AST_TAG_TYPE_KEY]->ToString()](node);
    }
    EvalDispatcher(void) = default;
    EvalDispatcher(const EvalDispatcher &) = delete;
    EvalDispatcher(EvalDispatcher &&) = delete;
};
