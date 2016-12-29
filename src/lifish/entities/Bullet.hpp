#pragma once

#include "Entity.hpp"

namespace Game {

class Collider;

/**
 * All this data is biunivocal with the bullet's id. A bullet's id simultaneously
 * defines its texture, size and spritesheet format.
 */
struct BulletData {
	/** The actual size of this bullet in pixels */
	unsigned short size;
	unsigned short nMotionFrames;  // up to 8/directionality for AxisBullet
	unsigned short nDestroyFrames; // up to 5
	/** Only valid for AxisBullets.
	 *  If 1 => this bullet has the same animation for any direction, up to 8.
	 *  If 2 => this bullet has 2 different animations when traveling UP/DOWN or
	 *          LEFT/RIGHT, up to 4 per direction.
	 *  If 4 => this bullet has different animations for each direction, up to 2.
	 *  The effective number of frames is established by nMotionFrames.
	 */
	unsigned short directionality;

	BulletData() : size(0), nMotionFrames(0), nDestroyFrames(0), directionality(0) {}
	BulletData(unsigned short size, unsigned short nMotionFrames,
			unsigned short nDestroyFrames, unsigned short directionality = 0)
		: size(size)
		, nMotionFrames(nMotionFrames)
		, nDestroyFrames(nDestroyFrames)
		, directionality(directionality)
	{}
};

/**
 * The information needed to construct a Bullet
 */
struct BulletInfo {
	/** Determines the bullet's data */
	unsigned short id;
	/** The damage to deal to the impacted Entity */
	int damage;
	/** Bullet speed, relative to bullets' base speed */
	float speed;
	/** How many pixels does this bullet travel; -1 means infinite. */
	float range;
};

/**
 * A bullet travels until it impacts with another Entity which is not transparent to it.
 */
class Bullet : public Game::Entity {
protected:
	const BulletInfo info;
	const BulletData data;
	/** The Entity that shot this bullet (optional) */
	const Game::Entity *const source;

	/** Whether this bullet already dealt its damage */
	bool dealtDamage = false;

	/** This should be implemented by child classes */
	Game::Collider *collider = nullptr;


	void _destroy();

public:
	explicit Bullet(const sf::Vector2f& pos, const Game::BulletInfo& info,
			const Game::Entity *const source = nullptr);

	const Game::Entity* getSource() const { return source; }

	bool hasDealtDamage() const { return dealtDamage; }
	/** Marks this bullet as 'already dealt damage', so that it's not applied more than once. */
	void dealDamage() { dealtDamage = true; }

	const Game::BulletInfo& getInfo() const { return info; }

	Game::Entity* init() override;
	void update() override;
};

}
