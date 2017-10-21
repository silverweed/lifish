#pragma once

namespace lif {

namespace ai {

// Thanks to https://stackoverflow.com/a/23742130
struct StateFunctionHelper {
	using type = std::function<StateFunctionHelper()>;
	StateFunctionHelper(type f) : func(f) {}
	operator type () { return func; }
	type func;
};

using StateFunction = StateFunctionHelper::type;

} // end namespace ai

} // end namespace lif
