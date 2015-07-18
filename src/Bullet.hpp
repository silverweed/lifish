#pragma once
/**
 * A bullet
 */

#include "MovingEntity.hpp"

namespace Game {

class Bullet : public Game::MovingEntity {
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
	sf::Vector2f origin;

	unsigned short damage;

	unsigned short nMotionFrames = 1,  // up to 2
		       nDestroyFrames = 4; // up to 5

	sf::Vector2f shift;

	const Game::Entity *source = nullptr;

	bool destroyed = false;

	bool _isTransparentTo(const Entity *const) const;
public:
	constexpr static float BASE_SPEED = 200.f;

	Bullet(const sf::Vector2f& pos, const Game::Direction dir, unsigned short id,
			float speed, unsigned short damage, short range = -1);

	void setSource(const Game::Entity *e) { source = e; }
	const Game::Entity* getSource() const { return source; }

	void move() override;
	void move(const Direction) override;

	void draw(sf::RenderTarget& window) override;

	bool hits(const sf::Vector2f& pos) const;

	unsigned short getDamage() const { return damage; }

	void destroy();
	bool isDestroyed() const { return destroyed && !animatedSprite.isPlaying(); }

	unsigned short getSize() const { return size; }
};

}
