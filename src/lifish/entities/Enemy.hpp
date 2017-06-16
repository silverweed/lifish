#pragma once

#include <memory>
#include <array>
#include <SFML/System.hpp>
#include "Entity.hpp"
#include "Attack.hpp"
#include "game.hpp"

namespace lif {

class Enemy;
class Collider;
class Animated;
class Clock;
class Killable;
class MovingAnimator;
class AI;
class AlienSprite;
class AxisSighted;
class AxisMoving;
class Shooting;
class AutoShooting;
class RegularEntityDeath;

struct EnemyInfo {
	unsigned short ai = 0;
	unsigned short speed = 0;
	Attack attack;
};

/**
 * This class provides a Drawable proxy to draw the regular enemy's sprite
 * or its AlienSprite according to the morphed state of the Enemy.
 */
class EnemyDrawableProxy : public sf::Drawable {
	const lif::Enemy& enemy;
	const lif::Animated& morphedAnim;

public:
	explicit EnemyDrawableProxy(const lif::Enemy& e);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

/** 
 * An Enemy is a MovingEntity which (usually) shoots towards players 
 * when they see them and is vulnerable to Bombs.
 */
class Enemy : public lif::Entity {

	friend class EnemyDrawableProxy;

	std::unique_ptr<lif::EnemyDrawableProxy> drawProxy;

protected:
	constexpr static unsigned short WALK_N_FRAMES = 4;
	constexpr static int YELL_DELAY = 1000;
	
	const unsigned short id;
	const lif::EnemyInfo info;

	lif::Shooting *shooting = nullptr;
	lif::AutoShooting *autoShooting = nullptr;
	lif::Collider *collider = nullptr;
	lif::Animated *animated = nullptr;
	lif::AxisMoving *moving = nullptr;
	lif::Killable *killable = nullptr;
	lif::MovingAnimator *movingAnimator = nullptr;
	lif::AxisSighted *sighted = nullptr;
	lif::AI *ai = nullptr;
	lif::RegularEntityDeath *death = nullptr;

	std::array<sf::Sprite, 4> shootFrame;

	lif::Clock *yellClock = nullptr,
		   *dashClock = nullptr;
	sf::Time morphDuration;

	/** True when the enemy is morphed into a harmless Alien during EXTRA game */
	bool morphed = false;
	lif::AlienSprite *alienSprite = nullptr;

	const float originalSpeed;

	/** Used to perform some actions before calling 'hurtByExplosions'.
	 *  @return true if 'hurtByExplosions' should NOT be called.
	 */
	virtual bool _checkCollision(lif::Collider& coll);
	void _checkShoot();
	/** @return Whether entity `e` is within our Shooting range or not */
	bool _inRange(const lif::Entity *const e) const;

public:
	constexpr static float BASE_SPEED = 75.f;

	unsigned short distanceWithNearestPlayer = -1; // "infinity"

	// TODO: eliminate need for `id`
	explicit Enemy(const sf::Vector2f& pos, unsigned short id, const lif::EnemyInfo& info);

	void setMorphed(bool b);
	bool isMorphed() const { return morphed; }

	void update() override;
	void setOrigin(const sf::Vector2f& pos) override {
		lif::Entity::setOrigin(pos);
		for (auto& frame : shootFrame)
			frame.setOrigin(pos);
	}
};

}
