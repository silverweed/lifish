#pragma once

#include <functional>
#include <utility>

namespace lif {

namespace ai {

/** A StateFunction is a function which does some computation and returns 
 *  another StateFunction (possibly the same).
 *  Users of this class should leverage the move constructor whenever possible
 *  to avoid useless copies. If a copy-construction is needed, pass stateFunction.f,
 *  which will use the base constructor.
 */
struct StateFunction {
	template <typename T>
	StateFunction(const T& f) : f(f) {}
	StateFunction(StateFunction&& s) = default;
	StateFunction(const StateFunction& s) = delete;
	StateFunction operator()() {
		return f();
	}
	StateFunction& operator=(StateFunction&& s) {
		f = std::move(s.f);
		return *this;
	}
	StateFunction& operator=(const StateFunction&) = delete;
	operator bool() const { return bool(f); }
	std::function<StateFunction()> f;
};

} // end namespace ai

} // end namespace lif
