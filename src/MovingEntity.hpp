#pragma once
/**
 * Entities which can move (enemies, players, bullets, ...)
 */

#include <string>
#include <array>
#include <ostream>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"
#include "Animated.hpp"

namespace Game {

enum class Direction {
	UP, LEFT, DOWN, RIGHT, NONE
};

std::ostream& operator<<(std::ostream& stream, const Direction& dir);

inline Direction oppositeDirection(const Direction dir) {
	switch (dir) {
	case Direction::UP:    return Direction::DOWN;
	case Direction::DOWN:  return Direction::UP;
	case Direction::LEFT:  return Direction::RIGHT;
	case Direction::RIGHT: return Direction::LEFT;
	default:               return Direction::NONE;
	}
}

class MovingEntity : public Game::Entity, public Game::Animated {
protected:
	constexpr static unsigned short WALK_N_FRAMES = 8;
	constexpr static unsigned short DEATH_N_FRAMES = 3;
	enum : unsigned short {
		ANIM_UP    = 0,
		ANIM_RIGHT = 1,
		ANIM_DOWN  = 2,
		ANIM_LEFT  = 3,
		ANIM_DEATH = 4,
		ANIM_WIN   = 5,
		ANIM_HURT  = 6
	};
	float speed;
	Game::Direction direction, prevDirection = Game::Direction::NONE;
	bool moving = false;
public:
	bool colliding = false;

	MovingEntity(sf::Vector2f pos, const std::string& texture_name);

	float getSpeed() const { return speed; }

	Game::Direction getDirection() const { return direction; }
	void setDirection(const Game::Direction dir) { direction = dir; }

	virtual void draw(sf::RenderTarget& window) override { Game::Animated::draw(window); }

	virtual void move();
	virtual void move(const Direction direction);
	virtual void stop();

	/** Realigns this entity to the grid.
	 *  If direction != NONE, put the entity back to the tile whence it was moving,
	 *  else just floor down x and y so that the coordinates are multiple of TILE_SIZE.
	 */
	void realign();

	bool isMoving() const { return moving; }
};

}
