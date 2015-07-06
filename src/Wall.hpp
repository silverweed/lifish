#pragma once
/**
 * Base class for walls.
 */
#include <string>
#include "FixedEntity.hpp"

namespace Game {

class Wall : public Game::FixedEntity {
public:
	Wall(const sf::Vector2f& pos, const std::string& texture_name)
		: FixedEntity(pos, texture_name)
	{
		transparentTo.players = false;
		transparentTo.enemies = false;
		transparentTo.bullets = false;
	}
};

}
