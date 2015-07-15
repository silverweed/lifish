#pragma once
/**
 * A coin
 */

#include "FixedEntity.hpp"
#include "Animated.hpp"

namespace Game {

class Coin : public Game::FixedEntity, public Game::Animated {
	constexpr static float GRAB_TIME = 3000; // ms	

	sf::Clock grabClock;
	bool grabbed = false;
public:
	constexpr static unsigned int VALUE = 150;

	Coin(const sf::Vector2f& pos);

	bool isBeingGrabbed() const { return grabbed; } 
	bool isGrabbed() const { return grabbed && grabClock.getElapsedTime().asMilliseconds() > GRAB_TIME; }
	void grab();

	void setOrigin(const sf::Vector2f& pos) override { Game::Animated::setOrigin(pos); }

	void draw(sf::RenderTarget& window) override;
};

}
