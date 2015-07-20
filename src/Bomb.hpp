#pragma once
/**
 * The players' bomb
 */

#include "FixedEntity.hpp"
#include "Animated.hpp"

namespace Game {

class Bomb : public Game::FixedEntity, public Game::Animated {
	sf::Clock fuseClock;
	unsigned short fuseTime;
	unsigned short radius;
	bool ignited = false;
	/** `true` if this bomb already spawned its explosion. */
	bool exploded = false;
public:
	constexpr static unsigned short DEFAULT_FUSE = 5000; // ms
	constexpr static unsigned short DEFAULT_RADIUS = 2;
	constexpr static unsigned short MAX_RADIUS = 4;

	Bomb(const sf::Vector2f& pos, const unsigned short fuseTime = DEFAULT_FUSE, const unsigned short radius = DEFAULT_RADIUS);

	void draw(sf::RenderTarget& window) override;
	void setOrigin(const sf::Vector2f& pos) override { Game::Animated::setOrigin(pos); }

	void ignite() { fuseClock.restart(); ignited = true; }

	bool isExploding() const { return ignited && fuseClock.getElapsedTime().asMilliseconds() >= fuseTime; }
	void setExploding() { fuseTime = 0.03; fuseClock.restart(); }

	bool isExploded() const { return exploded; }
	void blowUp() { exploded = true; }

	unsigned short getRadius() const { return radius; }
};

}
