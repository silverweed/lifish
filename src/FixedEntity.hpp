#pragma once

#include "Entity.hpp"

namespace Game {

/**
 * Entities which can't move (walls, teleporters, bonuses, coins, bosses)
 */
class FixedEntity : public Game::Entity {
public:
	FixedEntity(const sf::Vector2f& pos)
		: Entity(pos) {}
	FixedEntity(const sf::Vector2f& pos, const std::string& texture_name)
		: Entity(pos, texture_name) {}
};

}
