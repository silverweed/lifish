#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"

namespace lif {

class Animated;
class Collider;
class Killable;
class Clock;

/** The base class for Bosses */
class Boss : public lif::Entity {
	lif::Clock *explClock = nullptr,
	           *deathClock = nullptr;

protected:
	// The following components MUST be added by children
	lif::Collider *collider = nullptr;
	lif::Animated *animated = nullptr;
	lif::Killable *killable = nullptr;


	void _checkCollision(lif::Collider& coll);

	/** Resets deathClock */
	virtual void _kill();
	/** `true` while deathClock->getElapsedTime() < lif::conf::boss::DEATH_TIME */
	bool _killInProgress() const;

public:
	explicit Boss(const sf::Vector2f& pos);

	lif::Entity* init() override;
};

}
