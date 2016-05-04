#pragma once

#include "Entity.hpp"
#include "Animated.hpp"
#include "Clock.hpp"

namespace Game {

/**
 * A teleport can be used both by players and enemies.
 */
class Teleport : public Game::Entity {
	const static sf::Time COOLDOWN_TIME;

	/** The Teleport this one teleports to */
	Teleport *next_t = nullptr;

	bool disabled = false;
	/** Time to wait before reactivating */
	Game::Clock<1> *disableClock = nullptr;
	Game::Animated *animated = nullptr;

public:
	explicit Teleport(const sf::Vector2f& pos);

	Teleport* next() const { return next_t; }
	void linkTo(Teleport *nxt) { next_t = nxt; }

	/** Sets disableCount to its max value, disabling the teleport
	 *  until it drops to 0.
	 */
	void disable();

	bool isDisabled() const { return disabled; }

	void update() override;
};

}
