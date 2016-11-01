#pragma once

#include "Entity.hpp"

namespace Game {

class Collider;
class Killable;
class Clock;

/** The base class for Bosses */
class Boss : public Game::Entity {
protected:
	/** A Collider can be implemented by children */
	Game::Collider *collider = nullptr;
	Game::Killable *killable = nullptr;
	Game::Clock *explClock = nullptr,
	            *deathClock = nullptr;

	bool isHurt = false, wasHurt = false;


	void _checkCollision(Game::Collider& coll);

	/** Sets this boss as hurt for the next update, so that it can be e.g.
	 * drawn in a different way for that frame.
	 */
	void _hurt();
	/** Resets deathClock */
	virtual void _kill();
	/** `true` while deathClock->getElapsedTime() < Game::Conf::Boss::DEATH_TIME */
	bool _killInProgress() const;
public:
	explicit Boss(const sf::Vector2f& pos);

	void update() override;
};

}
