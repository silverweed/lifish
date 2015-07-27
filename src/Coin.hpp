#pragma once

#include "FixedEntity.hpp"
#include "Animated.hpp"
#include "Scored.hpp"

namespace Game {

/**
 * A coin can be taken by any player. Taking all coins triggers
 * EXTRA game, which morphs all enemies into harmless Aliens.
 */
class Coin : public Game::FixedEntity, public Game::Animated, public Game::Scored {
	constexpr static float GRAB_TIME = 3000; // ms	
	constexpr static unsigned int VALUE = 150;

	sf::Clock grabClock;
	bool grabbed = false;
public:
	Coin(const sf::Vector2f& pos);

	bool isBeingGrabbed() const { return grabbed; } 
	bool isGrabbed() const { return grabbed && grabClock.getElapsedTime().asMilliseconds() > GRAB_TIME; }
	void grab();

	void setOrigin(const sf::Vector2f& pos) override { Game::Animated::setOrigin(pos); }

	void draw(sf::RenderTarget& window) override;
};

}
