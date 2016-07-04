#pragma once

#include <array>
#include "Chronometer.hpp"
#include "Component.hpp"

namespace Game {

class Clock : public Game::Component {
	sftools::Chronometer clock;

public:
	explicit Clock(Game::Entity *const owner)
		: Game::Component(owner)
	{}

	sftools::Chronometer* getClock() {
		return &clock;
	}

	sf::Time restart() {
		return clock.restart();
	}

	sf::Time getElapsedTime() const {
		return clock.getElapsedTime();
	}
};

}
