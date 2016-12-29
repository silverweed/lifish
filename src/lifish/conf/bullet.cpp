#include "bullet.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"
#include "core.hpp"

std::map<unsigned short, const Game::BulletInfo> Game::Conf::Bullet::info = {
	//// Bullets ////
	// shot(size, motion frames, destroy frames, directionality)
	{ 1, Game::BulletInfo(7, 1, 4, 1) },
	// fireball
	{ 2, Game::BulletInfo(13, 1, 4, 1) },
	// MG shot
	{ 3, Game::BulletInfo(10, 1, 5, 4) },
	// lightbolt
	{ 4, Game::BulletInfo(13, 1, 4, 1) },
	// flame
	{ 5, Game::BulletInfo(32, 4, 0, 1) },
	// plasma
	{ 6, Game::BulletInfo(12, 1, 4, 1) },
	// magma
	{ 7, Game::BulletInfo(26, 1, 4, 4) },
	//// FreeBullets ////
	// star
	{ 101, Game::BulletInfo(20, 2, 5) }
};
