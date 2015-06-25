#pragma once
/**
 * Entities which can move (enemies, players, bullets, ...)
 */

#include "Entity.hpp"

namespace Game {

class MovingEntity : public Entity {
protected:
	float speed;
public:
	MovingEntity(sf::Vector2f pos, const std::string& texture_name)
		: Entity(pos, texture_name) {}

	virtual float getSpeed() const { return speed; }
};

}
