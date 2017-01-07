#include "TimeStats.hpp"
#include <exception>

using lif::Debug::TimeStats;

clock_t TimeStats::start(std::string name) {
	return timers[name] = clock();
}

double TimeStats::end(std::string name) {
	auto it = timers.find(name);
	if (it == timers.end())
		throw std::logic_error(name + ": end() called without start()!");

	return results[name] = double(clock() - it->second) / CLOCKS_PER_SEC;
}

double TimeStats::get(std::string name) const {
	{
		// Check if a result was already saved for this timer
		auto it = results.find(name);
		if (it != results.end())
			return it->second;
	}

	// Else, this timer is still running: return snapshot
	auto it = timers.find(name);
	if (it == timers.end())
		throw std::logic_error(name + ": get() called without start()!");

	return double(clock() - it->second) / CLOCKS_PER_SEC;
}

double TimeStats::safeGet(std::string name) const {
	try {
		return get(name);
	} catch (std::exception) {
		return -1;
	}
}

void TimeStats::set(std::string name, double amt) {
	results[name] = amt;
}

void TimeStats::flush() {
	timers.clear();
	results.clear();
}
