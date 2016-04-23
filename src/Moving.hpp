#pragma once

#include "Component.hpp"
#include "Direction.hpp"

namespace Game {

class Moving : public Game::Component {
protected:
	float speed;
	Game::Direction direction;

public:
	explicit Moving(Game::Entity *const owner, float speed);

	float getSpeed() const { return speed; }
	void setSpeed(const float _speed) { speed = _speed; }

	Game::Direction getDirection() const { return direction; }
	void setDirection(const Game::Direction dir); 
};

}
