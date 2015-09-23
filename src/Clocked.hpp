#pragma once

#include <initializer_list>
#include "Chronometer.hpp"

/**
 * An object with one or more pausable clocks
 */
namespace Game {

class Clocked {
	std::vector<sftools::Chronometer*> clocks;

protected:
	void _addClock(sftools::Chronometer* clock) {
		clocks.push_back(clock);
	}

public:
	Clocked(sftools::Chronometer* clock) {
		clocks.push_back(clock);
	}

	Clocked(std::initializer_list<sftools::Chronometer*> _clocks) {
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
