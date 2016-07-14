#pragma once

#include <string>
#include <SFML/System.hpp>
#include "Entity.hpp"

namespace Game {

/**
 * An unbreakable wall
 */
class FixedWall : public Game::Entity {
public:
	explicit FixedWall(const sf::Vector2f& pos, const unsigned short id);
};

}
