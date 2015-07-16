#pragma once
/**
 * Entities which can't move (walls, teleporters, powerups, coins, bosses)
 */
#include "Entity.hpp"

namespace Game {

class FixedEntity : public Game::Entity {
public:
	FixedEntity(const sf::Vector2f& pos)
		: Entity(pos) {}
	FixedEntity(const sf::Vector2f& pos, const std::string& texture_name)
		: Entity(pos, texture_name) {}
};

}
