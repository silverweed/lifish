#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "Entity.hpp"

namespace lif {

class Animated;
class Collider;
class Killable;
class Clock;

class Boss;

class BossDrawableProxy : public sf::Drawable {
	const lif::Boss& boss;
public:
	explicit BossDrawableProxy(const lif::Boss& b);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

/** The base class for Bosses */
class Boss : public lif::Entity {

	friend class lif::BossDrawableProxy;
	
	lif::BossDrawableProxy drawProxy;
	lif::Clock *explClock = nullptr,
	            *hurtClock = nullptr,
	            *deathClock = nullptr;

protected:
	// The following components MUST be added by children
	lif::Collider *collider = nullptr;
	lif::Animated *animated = nullptr;
	lif::Killable *killable = nullptr;


	void _checkCollision(lif::Collider& coll);

	/** Sets this boss as hurt for the next update, so that it can be e.g.
	 * drawn in a different way for that frame.
	 */
	void _hurt();
	/** Resets deathClock */
	virtual void _kill();
	/** `true` while deathClock->getElapsedTime() < lif::conf::boss::DEATH_TIME */
	bool _killInProgress() const;
public:
	explicit Boss(const sf::Vector2f& pos);

	lif::Entity* init() override;
};

}
