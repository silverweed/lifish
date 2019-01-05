#pragma once

#include "Entity.hpp"
#include "collision_layers.hpp"

namespace lif {

class Collider;
class Moving;

/**
 * All this data is biunivocal with the bullet's dataId. A bullet's dataId simultaneously
 * defines its texture, size and spritesheet format.
 */
struct BulletData {
	/** Name of the file containing the bullet's spritesheet (relative to graphics/) */
	std::string filename;
	/** The actual size of this bullet in pixels */
	unsigned short size;
	/** Number of frames of the motion animation */
	unsigned short nMotionFrames;
	/** Number of frames of the destruction animation */
	unsigned short nDestroyFrames;
	/** Only valid for AxisBullets.
	 *  If 1 => this bullet has the same animation for any direction.
	 *  If 2 => this bullet has 2 different animations when traveling UP/DOWN or LEFT/RIGHT
	 *  If 4 => this bullet has different animations for each direction.
	 *  The effective number of frames is established by nMotionFrames, and is the same for all directions.
	 */
	unsigned short directionality;
};

/**
 * The information needed to construct a Bullet.
 */
struct BulletInfo {
	/** Determines the bullet's data */
	unsigned short dataId = 0;
	/** The damage to deal to the impacted Entity */
	int damage = 1;
	/** Bullet speed, relative to players' base speed (1 means "as fast as players").
	 *  For accelerating bullets, it's the _start_ speed.
	 */
	float speed = 1;
	/** How many pixels does this bullet travel; -1 means infinite. */
	float range = -1;

	/** The bullet's collision layer */
	lif::c_layers::Layer cLayer = lif::c_layers::ENEMY_BULLETS;

	BulletInfo(unsigned short dataId, int damage, float speed,
			float range = -1, lif::c_layers::Layer cLayer = lif::c_layers::ENEMY_BULLETS)
		: dataId(dataId), damage(damage), speed(speed), range(range), cLayer(cLayer)
	{}
	BulletInfo() = default;
	BulletInfo(const BulletInfo&) = default;
};

/**
 * A bullet travels until it impacts with another Entity which is not transparent to it.
 */
class Bullet : public lif::Entity {
protected:
	const BulletInfo info;
	const BulletData data;
	/** The Entity that shot this bullet (optional) */
	const lif::Entity *const source;

	/** The actual bullet speed, derived from info.speed and info.acceleration */
	float speed;

	/** Whether this bullet already dealt its damage */
	bool dealtDamage = false;

	/** This should be implemented by child classes */
	lif::Collider *collider = nullptr;
	/** This should be implemented by child classes */
	lif::Moving *moving = nullptr;


	/** Common setup for Free and GuidedBullets */
	void _setup();
	void _destroy();

public:
	explicit Bullet(const sf::Vector2f& pos, const lif::BulletInfo& info,
			const lif::Entity *const source = nullptr);

	const lif::Entity* getSource() const { return source; }

	bool hasDealtDamage() const { return dealtDamage; }
	/** Marks this bullet as 'already dealt damage', so that it's not applied more than once. */
	void dealDamage() { dealtDamage = true; }

	const lif::BulletInfo& getInfo() const { return info; }

	lif::Entity* init() override;
	void update() override;
};

}
