#pragma once

#include "Moving.hpp"
#include "Direction.hpp"

namespace Game {

/**
 * An object moving only parallel to axes */
class AxisMoving : public Game::Moving {
	Game::Direction direction;
	Game::Direction prevDirection;
	
	void _realign();

public:
	explicit AxisMoving(Game::Entity *const owner, float speed, 
			Game::Direction dir = Game::Direction::NONE);

	Game::Direction getDirection() const { return direction; }
	void setDirection(Game::Direction dir) { direction = dir; }

	void stop() override;

	void update() override;
};

}
