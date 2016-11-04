#include "Counter.hpp"
#include <exception>

using Game::Debug::Counter;

int Counter::inc(std::string name) {
	return ++counts[name]; 
}

void Counter::reset(std::string name) {
	counts[name] = 0;
}

int Counter::get(std::string name) const {
	auto it = counts.find(name);
	if (it == counts.end())
		throw std::invalid_argument(name + ": get() called without inc()!");
	return it->second;
}

int Counter::safeGet(std::string name) const {
	try {
		return get(name);
	} catch (std::exception) {
		return -1;
	}
}
