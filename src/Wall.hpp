#pragma once
/**
 * Base virtual class for walls.
 */
#include "FixedEntity.hpp"

namespace Game {

class Wall : public Game::FixedEntity {
public:
	Wall(sf::Vector2f pos)
		: FixedEntity(pos) {}
};

}
