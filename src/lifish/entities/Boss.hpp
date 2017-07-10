#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"

namespace lif {

class Animated;
class Collider;
class Killable;
class Clock;

/** The base class for Bosses.
 *  IMPORTANT: all subclasses MUST define the following components:
 *	- Collider
 *	- Animated
 *	- Lifed (only if the Boss can collide and be damaged)
 *	- Sounded, with at least the following sounds:
 *		- hurt (only if the Boss can collide and be damaged)
 */
class Boss : public lif::Entity {
	lif::Clock *explClock = nullptr,
	           *deathClock = nullptr;

protected:
	lif::Killable *killable = nullptr;
	// The following components MUST be added by children.
	lif::Collider *collider = nullptr;
	lif::Animated *animated = nullptr;


	void _checkCollision(lif::Collider& coll);
	/** Resets deathClock */
	virtual void _kill();
	/** `true` while deathClock->getElapsedTime() < lif::conf::boss::DEATH_TIME */
	bool _killInProgress() const;
	/** Convenient function to add a collider which just calls _checkCollision */
	void _addDefaultCollider(const sf::Vector2f& size);

public:
	explicit Boss(const sf::Vector2f& pos);

	lif::Entity* init() override;
};

}
