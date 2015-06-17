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
	float getSpeed() const { return speed; }
};

}
