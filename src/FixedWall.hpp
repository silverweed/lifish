#pragma once
/**
 * An unbreakable wall
 */
#include <string>
#include "Wall.hpp"

namespace Game {

class FixedWall : public Wall {
public:
	FixedWall(sf::Vector2f pos, const unsigned short id);
};

}
