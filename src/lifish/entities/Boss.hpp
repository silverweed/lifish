#pragma once

#include "Entity.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

namespace lif {

class Collider;
class Drawable;
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
	           *deathClock = nullptr,
	           *blinkClock = nullptr;

protected:
	lif::Killable *killable = nullptr;
	// The following components MUST be added by children.
	lif::Drawable *drawable = nullptr;
	lif::Collider *collider = nullptr;

	/** If `false`, boss cannot be hurt by bombs. */
	bool vulnerable = true;


	void _checkCollision(lif::Collider& coll);

	/** Resets deathClock */
	virtual void _kill();

	/** Convenient function to add a collider which just calls _checkCollision */
	void _addDefaultCollider(const sf::Vector2f& size);

	virtual void update() override;
public:
	explicit Boss(const sf::Vector2f& pos);

	lif::Entity* init() override;
};

}
