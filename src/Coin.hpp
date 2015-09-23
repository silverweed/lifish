#pragma once

#include "Animated.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"

namespace Game {

/**
 * A coin can be taken by any player. Taking all coins triggers
 * EXTRA game, which morphs all enemies into harmless Aliens.
 */
class Coin
	: public Game::Animated
	, public Game::Scored
	, public Game::Sounded 
{
	constexpr static float GRAB_TIME = 3000; // ms	
	constexpr static unsigned int VALUE = 150;

	sftools::Chronometer grabClock;
	bool grabbed = false;

public:
	Coin(const sf::Vector2f& pos);

	bool isBeingGrabbed() const { return grabbed; } 
	bool isGrabbed() const;
	void grab();

	void draw(sf::RenderTarget& window) override;
};

}
