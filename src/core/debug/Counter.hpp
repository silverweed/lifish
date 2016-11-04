#pragma once

#include <map>
#include <string>

namespace Game {

namespace Debug {

/**
 * A simple key-value counter.
 */
class Counter {
	std::map<std::string, int> counts;
public:
	/** Increments the counter `name`, creating it if non-existing. 
	 *  @return The updated count.
	 */
	int inc(std::string name);
	/** Resets the counter `name`. */
	void reset(std::string name);
	/** Returns the count of the counter `name`, throwing if non-existing. */
	int get(std::string name) const; 
	/** Like `get()`, but returns -1 instead of throwing. */
	int safeGet(std::string name) const;
};

}

}
