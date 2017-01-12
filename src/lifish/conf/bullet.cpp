#include "bullet.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"
#include "core.hpp"

std::map<unsigned short, const lif::BulletData> lif::conf::bullet::data = {
	//// Bullets ////
	// shot(size, motion frames, destroy frames, directionality)
	{ 1, lif::BulletData(7, 1, 4, 1) },
	// fireball
	{ 2, lif::BulletData(13, 1, 4, 1) },
	// MG shot
	{ 3, lif::BulletData(10, 1, 5, 4) },
	// lightbolt
	{ 4, lif::BulletData(13, 1, 4, 1) },
	// flame
	{ 5, lif::BulletData(32, 4, 0, 1) },
	// plasma
	{ 6, lif::BulletData(12, 1, 4, 1) },
	// magma
	{ 7, lif::BulletData(26, 1, 4, 4) },
	//// FreeBullets ////
	// star
	{ 101, lif::BulletData(20, 2, 5) },
	// spirit bullets
	{ 102, lif::BulletData(16, 2, 3) }
};
