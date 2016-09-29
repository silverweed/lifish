#pragma once

#include "Entity.hpp"

namespace Game {

class Animated;

/**
 * The explosions appearing when a Boss dies.
 */
class BossExplosion : public Game::Entity {
	Game::Animated *animated = nullptr;

public:
	explicit BossExplosion(const sf::Vector2f& pos);
};

}
