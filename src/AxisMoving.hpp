#pragma once

#include "Moving.hpp"
#include "Direction.hpp"

namespace Game {

class LevelManager;

/**
 * An object moving only parallel to axes 
 */
class AxisMoving : public Game::Moving {
	Game::Direction direction;
	Game::Direction prevDirection;
	sf::Vector2i prevAlign;


	void _ensureAlign();

public:
	explicit AxisMoving(Game::Entity *const owner, float speed, 
			Game::Direction dir = Game::Direction::NONE);

	Game::Direction getDirection() const { return direction; }
	Game::Direction getPrevDirection() const { return prevDirection; }
	const sf::Vector2i& getPrevAlign() const { return prevAlign; }

	void setDirection(Game::Direction dir);
	void turn(short straightAngles, bool clockwise);
	void setPrevAlign(const sf::Vector2i& a) { prevAlign = a; }

	void realign();
	void stop() override;
	void update() override;

	bool canGo(const Game::Direction dir, const Game::LevelManager *const lm) const;
};

}
