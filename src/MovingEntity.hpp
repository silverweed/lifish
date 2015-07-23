#pragma once
/**
 * Entities which can move (enemies, players, bullets, ...)
 */

#include <string>
#include <array>
#include <ostream>
#include "Entity.hpp"
#include "Animated.hpp"
#include "Teleport.hpp"

namespace Game {

class LevelRenderer;

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
	/** This unit's base speed */
	float speed = 0.f;
	Game::Direction direction = Game::Direction::NONE, prevDirection = Game::Direction::NONE;
	bool moving = false;

	bool hurt = false, hurtAnimPrepared = false;
	bool dead = false, deathAnimPrepared = false;
	short remainingLives = 0;

	int shieldTime = -1; // milliseconds; negative means "no shield"
	sf::Clock shieldClock;

	int speedyTime = -1; // milliseconds; negative means "no speedy"
	sf::Clock speedyClock;

	constexpr static unsigned short DEATH_TIME = 2000; // ms
	sf::Clock deathClock;

	virtual bool _isTransparentTo(const Entity *const e) const = 0;
	virtual void _ensureAlign();
public:
	bool colliding = false;
	/** Whether this entity is affected by Teleports or not */
	bool canTeleport = true;
	Game::Teleport *onTeleport = nullptr;
	sf::Vector2i prevAlign = sf::Vector2i(-1, -1);
	float distTravelled = 0;

	MovingEntity(const sf::Vector2f& pos, const std::string& texture_name);

	float getSpeed() const { return speed; }
	void setSpeed(const float _speed) { speed = _speed; }

	Game::Direction getDirection() const { return direction; }
	void setDirection(const Game::Direction dir); 

	/** Use Animated::draw, not Entity's */
	virtual void draw(sf::RenderTarget& window) override;
	void setOrigin(const sf::Vector2f& _origin) override { Game::Animated::setOrigin(_origin); }

	virtual void move();
	virtual void move(const Direction direction);
	virtual void stop();

	/** Realigns this entity to the grid.
	 *  If direction != NONE, put the entity back to the tile whence it was moving,
	 *  else just floor down x and y so that the coordinates are multiple of TILE_SIZE.
	 */
	void realign();

	bool isMoving() const { return moving; }

	bool canGo(const Game::Direction direction, const Game::LevelRenderer *const lr) const; 

	void setPosition(const sf::Vector2f& _pos) override {
		Game::Animated::setPosition(_pos);
		Game::Entity::setPosition(_pos);
	}

	void setHurt(const bool b);
	bool isHurt() const { return hurt; }

	virtual void prepareHurtAnimation();
	virtual bool playHurtAnimation();
	virtual void prepareDeathAnimation();
	virtual bool playDeathAnimation();

	void giveShield(const int shieldMs) {
		shieldTime = shieldMs;
		shieldClock.restart();
	}
	bool hasShield() const { return shieldTime > 0 && shieldClock.getElapsedTime().asMilliseconds() <= shieldTime; }

	/** Doubles this unit's speed for `speedyMs` ms. */
	void giveSpeedy(const int speedyMs) {
		speedyTime = speedyMs;
		speedyClock.restart();
	}

	bool isDying() const { return dead; }
	void kill();

	short getRemainingLives() const { return remainingLives; }
};

}
