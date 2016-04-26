#pragma once

#include "Component.hpp"
#include "Direction.hpp"

namespace Game {

class BulletPresets;

/**
 * A bullet can only travel towards a single direction until it
 * impacts with another Entity which is not transparentTo.bullets.
 * Cannot travel diagonally.
 */
class Bullet : public Game::Entity {
	friend class Game::BulletPresets;

protected:
	/** If 1 => this bullet has the same animation for any direction, up to 8.
	 *  If 2 => this bullet has 2 different animations when traveling UP/DOWN or
	 *          LEFT/RIGHT, up to 4 per direction.
	 *  If 4 => this bullet has different animations for each direction, up to 2.
	 *  The effective number of frames is established by nMotionFrames.
	 */
	unsigned short directionality = 1;
	
	/** The actual size of this bullet in pixels */
	unsigned short size;

	/** How many tiles does this bullet travel; -1 means infinite. */
	short range;

	/** The position this bullet was shot from */
	const sf::Vector2f origin;
	/** The Entity that shot this bullet */
	const Game::Entity *source;

	unsigned short damage;

	unsigned short nMotionFrames = 1,  // up to 2
		       nDestroyFrames = 4; // up to 5

	//sf::Vector2f shift;

	//bool destroyed = false;

	/** This constructor is used by BossBullet */
	//Bullet(const sf::Vector2f& pos, const std::string& texture_name);

	void _destroy();

public:
	constexpr static float BASE_SPEED = 200.f;

	/** This is the constructor used by regular bullets */
	Bullet(const sf::Vector2f& pos, const Game::Direction dir, unsigned short id,
			float speed, unsigned short damage, short range = -1);

	void setSource(const Game::Entity *e) { source = e; }
	const Game::Entity* getSource() const { return source; }

	//bool hits(const sf::Vector2f& pos) const;

	unsigned short getDamage() const { return damage; }

	//void destroy();
	//bool isBeingDestroyed() const { return destroyed; }
	//bool isDestroyed() const { return destroyed && !animatedSprite.isPlaying(); }

	//unsigned short getSize() const { return size; }
};

class BulletPresets {
	static void setup(Game::Bullet& b, unsigned short id);
};

}
