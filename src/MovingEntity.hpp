#pragma once
/**
 * Entities which can move (enemies, players, bullets, ...)
 */

#include <string>
#include <array>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"
#include "Animated.hpp"

namespace Game {

/** Directions; NOTE: it's mandatory that (dir + 2) % 4 == opposite direction */
enum Direction : unsigned short { 
	DIR_UP    = 0,
	DIR_RIGHT = 1, 
	DIR_DOWN  = 2, 
	DIR_LEFT  = 3,
	DIR_NONE  = 4
};

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
	Game::Direction direction, prevDirection = Game::DIR_NONE;
	bool moving = false;
public:
	/** Indexed with DIR_UP, etc */
	std::array<bool, 4> colliding;

	MovingEntity(sf::Vector2f pos, const std::string& texture_name);

	float getSpeed() const { return speed; }

	Game::Direction getDirection() const { return direction; }
	void setDirection(const Game::Direction dir) { direction = dir; }

	virtual void draw(sf::RenderTarget& window) override { Game::Animated::draw(window); }

	virtual void move();
	virtual void move(const Direction direction);
	virtual void stop();

	bool isMoving() const { return moving; }
};

}
