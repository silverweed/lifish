#pragma once

#include "Component.hpp"
#include "Direction.hpp"
#include "Position.hpp"

namespace Game {

class Moving : public Game::Component {
protected:
	float speed;
	Game::Direction direction;
	Game::Position<float> *position;

public:
	explicit Moving(Game::Entity *const owner);

	float getSpeed() const { return speed; }
	virtual void setSpeed(const float _speed) { speed = _speed; }

	Game::Direction getDirection() const { return direction; }
	virtual void setDirection(const Game::Direction dir); 
};

}
