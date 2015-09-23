#pragma once

#include <string>
#include "NonAnimated.hpp"

namespace Game {

/**
 * An unbreakable wall
 */
class FixedWall : public Game::NonAnimated {
public:
	FixedWall(sf::Vector2f pos, const unsigned short id);
};

}
