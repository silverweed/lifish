#pragma once

#include "Temporary.hpp"

namespace Game {

/**
 * The explosions appearing when a Boss dies.
 */
class BossExplosion : public Game::Temporary {
public:
	BossExplosion(const sf::Vector2f& pos);
};

}
