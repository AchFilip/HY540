class ValueStack {

	protected:
	std::list<Value> valueStack;

	public:
	bool	IsEmpty (void) const  { return valueStack.empty(); }
	void	Clear (void)	{ valueStack.clear(); }
	void	Push (const Value& v) { valueStack.push_front(v); }
	void	Pop (const Value& v)  { assert(!IsEmpty()); valueStack.pop_front(); }
	auto	Top (void) const -> const Value& 
		   { assert(!IsEmpty()); return valueStack.front(); }
	auto	GetTopAndPop (void) -> const Value {
		    assert(!IsEmpty()); 
		    auto v = valueStack.front();
		    valueStack.pop_front();
		    return v;
		}

	ValueStack (void) = default;
	ValueStack  (const ValueStack&) = delete;
	ValueStack  (ValueStack&&) = delete;
};
