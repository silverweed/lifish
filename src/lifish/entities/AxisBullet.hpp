#pragma once

#include "Bullet.hpp"
#include "Direction.hpp"

namespace Game {

/** A Bullet which travels along axes */
class AxisBullet : public Game::Bullet {
public:
	/** Constructs a Bullet with a source Entity (using that Entity's position) */
	explicit AxisBullet(const sf::Vector2f& pos, const Game::Entity *const source,
			const Game::Direction dir, const Game::Attack& attack);
};

}
