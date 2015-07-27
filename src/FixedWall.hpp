#pragma once

#include <string>
#include "Wall.hpp"

namespace Game {

/**
 * An unbreakable wall
 */
class FixedWall : public Game::Wall {
public:
	FixedWall(sf::Vector2f pos, const unsigned short id);
};

}
