#pragma once

#include "Moving.hpp"
#include "Direction.hpp"

namespace Game {

class LevelManager;

/**
 * An object moving only parallel to axes */
class AxisMoving : public Game::Moving {
	Game::Direction direction;
	Game::Direction prevDirection;
	sf::Vector2f prevAlign;
	
	void _realign();

public:
	explicit AxisMoving(Game::Entity *const owner, float speed, 
			Game::Direction dir = Game::Direction::NONE);

	Game::Direction getDirection() const { return direction; }
	Game::Direction getPrevDirection() const { return prevDirection; }
	void setDirection(Game::Direction dir);
	void turn(short straightAngles, bool clockwise);

	void stop() override;

	void update() override;

	bool canGo(const Game::Direction dir, const Game::LevelManager *const lm) const;
};

}
