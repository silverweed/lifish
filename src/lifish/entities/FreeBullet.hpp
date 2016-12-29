#pragma once

#include "Bullet.hpp"

namespace Game {

/** A Bullet which travels along any angle */
class FreeBullet : public Game::Bullet {
public:
	/** Constructs a Bullet with a source Entity (using that Entity's position), 
	 *  traveling at `angle` radians from the vertical axis (CW).
	 */
	explicit FreeBullet(const sf::Vector2f& pos, float angle,
			const Game::BulletInfo& info,
			const Game::Entity *const source = nullptr);

};

}
