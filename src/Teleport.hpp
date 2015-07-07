#pragma once
/**
 * A teleport
 */

#include "FixedEntity.hpp"
#include "Animated.hpp"

namespace Game {

class Teleport : public Game::FixedEntity, public Game::Animated {
	constexpr static unsigned int COOLDOWN_FRAMES = 3000;

	/** The Teleport this one teleports to */
	Teleport *next_t = nullptr;
	/** Number of frames to wait before reactivating */
	int disableCount = 0;
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

	bool isDisabled() const { return disableCount > 0; }

	/** Lower the disableCount by 1. */
	void tick();
};

}
