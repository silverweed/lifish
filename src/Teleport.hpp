#pragma once

#include "FixedEntity.hpp"
#include "Animated.hpp"

namespace Game {

/**
 * A teleport can be used both by players and enemies.
 */
class Teleport : public Game::FixedEntity, public Game::Animated {
	constexpr static unsigned int COOLDOWN_TIME = 1000; // milliseconds

	/** The Teleport this one teleports to */
	Teleport *next_t = nullptr;

	bool disabled = false;
	/** Time to wait before reactivating */
	sf::Clock disableClock;
public:
	Teleport(const sf::Vector2f& pos);

	void draw(sf::RenderTarget& window) override;
	void setOrigin(const sf::Vector2f& origin) override { Game::Animated::setOrigin(origin); }

	Teleport* next() const { return next_t; }
	void linkTo(Teleport *nxt) { next_t = nxt; }

	/** Sets disableCount to its max value, disabling the teleport
	 *  until it drops to 0.
	 */
	void disable();

	bool isDisabled();
};

}
