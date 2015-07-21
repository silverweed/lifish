#pragma once
/**
 * The explosions appearing when a Boss dies.
 */

#include "Temporary.hpp"

namespace Game {

class BossExplosion : public Game::Temporary {
public:
	BossExplosion(const sf::Vector2f& pos);
};

}
