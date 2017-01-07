#pragma once

#include <string>
#include <map>
#include <ctime>

namespace lif {

namespace debug {

/**
 * Contains time statistics for performance analysis
 */
class TimeStats {
	/** Contains the starting points in ticks */
	std::map<std::string, clock_t> timers;
	/** Contains the results stored by calling `end` */
	std::map<std::string, double> results;

public:
	/** Sets the starting time for timer `timerName`. Resets previous, if present. 
	 *  @return the saved tick.
	 */
	clock_t start(std::string timerName);
	/** Sets the ending time for `timerName` and collects (end-start). 
	 *  @return the result.
	 */
	double end(std::string timerName);
	/** Retreives the result of `timerName`. If `end()` wasn't called yet, returns (cur_time - start).
	 *  If `start()` wasn't called either, throws.
	 */
	double get(std::string timerName) const;
	/** Like `get()`, but returns -1 instead of throwing. */
	double safeGet(std::string timerName) const;
	/** Manually sets results of `timerName` to `amt`. */
	void set(std::string timerName, double amt);
	/** Resets all timers and results */
	void flush();
};

}

}
