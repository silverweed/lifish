#pragma once
/**
 * Entities which can't move (walls, teleporters, powerups, coins, bosses)
 */
#include "Entity.hpp"

namespace Game {

class FixedEntity : public Entity {
public:
	FixedEntity(sf::Vector2f pos)
		: Entity(pos) {}

	FixedEntity(sf::Vector2f pos, const std::string& texture_name)
		: Entity(pos, texture_name) {}
};

}
