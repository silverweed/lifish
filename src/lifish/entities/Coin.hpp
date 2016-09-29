#pragma once

#include <SFML/System.hpp>
#include "Entity.hpp"

namespace Game {

class Clock;
class Grabbable;
class Animated;

/**
 * A coin can be taken by any player. Taking all coins triggers
 * EXTRA game, which morphs all enemies into harmless Aliens.
 */
class Coin : public Game::Entity {
	const static sf::Time GRAB_TIME; 
	constexpr static unsigned int VALUE = 150;

	Game::Clock *grabClock;
	Game::Grabbable *grabbable = nullptr;

	Game::Animated *animated = nullptr;

	void _grab();

public:
	explicit Coin(const sf::Vector2f& pos);
};

}
