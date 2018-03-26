#pragma once

#include <array>
#include "Chronometer.hpp"
#include "Component.hpp"

namespace lif {

class Clock : public lif::Component {
	sftools::Chronometer clock;

public:
	COMP_NOT_UNIQUE

	explicit Clock(lif::Entity& owner)
		: lif::Component(owner)
	{
		_declComponent<Clock>();
	}

	sf::Time restart() {
		return clock.restart();
	}

	sf::Time getElapsedTime() const {
		return clock.getElapsedTime();
	}

	sf::Time pause() {
		return clock.pause();
	}

	sf::Time resume() {
		return clock.resume();
	}

	sf::Time add(sf::Time time) {
		return clock.add(time);
	}

	bool isRunning() const {
		return clock.isRunning();
	}
};

}
