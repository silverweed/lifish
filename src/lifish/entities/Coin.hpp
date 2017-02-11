#pragma once

#include <SFML/System.hpp>
#include "Entity.hpp"

namespace lif {

class Clock;
class Grabbable;
class Animated;

/**
 * A coin can be taken by any player. Taking all coins triggers
 * EXTRA game, which morphs all enemies into harmless Aliens.
 */
class Coin : public lif::Entity {

	lif::Clock *grabClock;
	lif::Grabbable *grabbable = nullptr;

	lif::Animated *animated = nullptr;

	void _grab();

public:
	explicit Coin(const sf::Vector2f& pos);
};

}
