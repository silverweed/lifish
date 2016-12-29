#pragma once

#include "Bullet.hpp"

namespace Game {

class FreeBulletPresets;

/** A Bullet which travels along any angle */
class FreeBullet : public Game::Bullet {
public:
	/** Constructs a Bullet with a source Entity (using that Entity's position), 
	 *  traveling at `angle` radians from the vertical axis (CW).
	 */
	explicit FreeBullet(const sf::Vector2f& pos, const Game::Entity *const source, 
			const double angle, const Game::Attack& attack);

};

}
