#include "bullet.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"
#include "core.hpp"

std::map<unsigned, const lif::BulletData> lif::conf::bullet::data = {
	//// AxisBullets ////
	// dataId, {filename, size, motionFrames, destroyFrames, directionality}
	// shot
	{ 1, { "shot.png", 7, 1, 4, 1 } },
	// fireball
	{ 2, { "shot.png", 13, 1, 4, 1 } },
	// MG shot
	{ 3, { "mgshot.png", 10, 1, 5, 4 } },
	// lightbolt
	{ 4, { "shot.png", 13, 1, 4, 1 } },
	// flame
	{ 5, { "shot.png", 32, 4, 0, 1 } },
	// plasma
	{ 6, { "shot.png", 12, 1, 4, 1 } },
	// magma
	{ 7, { "shot.png", 26, 1, 4, 4 } },
	// rocket
	{ 8, { "shot.png", /*TODO*/32, 2, 4, 4 } },
	// grenade
	{ 9, { "grenade.png", 32, 1, 1, 1 } },
	//// FreeBullets ////
	// star
	{ 101, { "shot.png", 20, 2, 5, 0 } },
	// spirit bullets
	{ 102, { "spirit_bullets.png", 16, 2, 3, 0 } },
	// missile
	{ 103, { "missile.png", 32, 4, 4, 1 } },
};
