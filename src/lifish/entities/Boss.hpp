#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"

namespace Game {

class Animated;
class Collider;
class Killable;
class Clock;

class Boss;

class BossDrawableProxy : public sf::Drawable {
	const Game::Boss& boss;
public:
	explicit BossDrawableProxy(const Game::Boss& b);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

/** The base class for Bosses */
class Boss : public Game::Entity {

	friend class Game::BossDrawableProxy;
	
	Game::BossDrawableProxy drawProxy;
	Game::Clock *explClock = nullptr,
	            *hurtClock = nullptr,
	            *deathClock = nullptr;

protected:
	// The following components MUST be added by children
	Game::Collider *collider = nullptr;
	Game::Animated *animated = nullptr;
	Game::Killable *killable = nullptr;


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

	Game::Entity* init() override;
};

}
