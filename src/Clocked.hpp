#pragma once

#include <initializer_list>
#include "Chronometer.hpp"

namespace Game {

/**
 * An object with one or more pausable clocks
 */
class Clocked {
	std::vector<sftools::Chronometer*> clocks;

protected:
	void _addClock(sftools::Chronometer* clock) {
		clocks.push_back(clock);
	}

public:
	explicit Clocked(sftools::Chronometer* clock) {
		clocks.push_back(clock);
	}

	explicit Clocked(std::initializer_list<sftools::Chronometer*> _clocks) {
		for (auto& c : _clocks)
			clocks.push_back(c);
	}

	virtual void pauseClock() {
		for (auto& clock : clocks)
			clock->pause();
	}

	virtual void resumeClock() {
		for (auto& clock : clocks)
			clock->resume();
	}

	virtual void resetClock() {
		for (auto& clock : clocks)
			clock->reset(true);
	}
};

}
