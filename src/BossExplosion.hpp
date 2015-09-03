#pragma once

#include "Temporary.hpp"
#include "Sounded.hpp"

namespace Game {

/**
 * The explosions appearing when a Boss dies.
 */
class BossExplosion : public Game::Temporary, public Game::Sounded {
public:
	BossExplosion(const sf::Vector2f& pos);
};

}
