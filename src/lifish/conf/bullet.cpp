#include "bullet.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"
#include "core.hpp"

std::map<unsigned short, const Game::BulletData> Game::Conf::Bullet::data = {
	//// Bullets ////
	// shot(size, motion frames, destroy frames, directionality)
	{ 1, Game::BulletData(7, 1, 4, 1) },
	// fireball
	{ 2, Game::BulletData(13, 1, 4, 1) },
	// MG shot
	{ 3, Game::BulletData(10, 1, 5, 4) },
	// lightbolt
	{ 4, Game::BulletData(13, 1, 4, 1) },
	// flame
	{ 5, Game::BulletData(32, 4, 0, 1) },
	// plasma
	{ 6, Game::BulletData(12, 1, 4, 1) },
	// magma
	{ 7, Game::BulletData(26, 1, 4, 4) },
	//// FreeBullets ////
	// star
	{ 101, Game::BulletData(20, 2, 5) }
};
