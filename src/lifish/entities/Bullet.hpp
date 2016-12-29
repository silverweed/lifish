#pragma once

#include "Entity.hpp"

namespace Game {

class Collider;
struct Attack;

struct BulletInfo {
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

	BulletInfo() : size(0), nMotionFrames(0), nDestroyFrames(0), directionality(0) {}
	BulletInfo(unsigned short size, unsigned short nMotionFrames,
			unsigned short nDestroyFrames, unsigned short directionality = 0)
		: size(size)
		, nMotionFrames(nMotionFrames)
		, nDestroyFrames(nDestroyFrames)
		, directionality(directionality)
	{}
};

/**
 * A bullet travels until it impacts with another Entity which is not transparent to it.
 */
class Bullet : public Game::Entity {
protected:
	/** This is univocally determined by the bullet's id */
	const BulletInfo info;
	/** The position this bullet was shot from (used for range) */
	const sf::Vector2f origin;
	/** The Entity that shot this bullet */
	const Game::Entity *const source;

	/** Whether this bullet already dealt its damage */
	bool dealtDamage = false;
	/** The damage dealt to the impacted Entity */
	unsigned short damage;
	/** How many pixels does this bullet travel; -1 means infinite. */
	float range;

	/** This should be implemented by child classes */
	Game::Collider *collider = nullptr;

	/** This constructor is used by BossBullet */
	//Bullet(const sf::Vector2f& pos, const std::string& texture_name);

	void _destroy();

public:
	/** Constructs a Bullet without a source (must specify the position) */
	explicit Bullet(const sf::Vector2f& pos, const Game::Attack& attack);

	/** Constructs a Bullet with a source Entity */
	explicit Bullet(const sf::Vector2f& pos, const Game::Entity *const source, const Game::Attack& attack);

	const Game::Entity* getSource() const { return source; }

	bool hasDealtDamage() const { return dealtDamage; }
	void dealDamage() { dealtDamage = true; }
	unsigned short getDamage() const { return damage; }

	Game::Entity* init() override;
	void update() override;
};

}
