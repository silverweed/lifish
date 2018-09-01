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
	{ 2, { "fireball.png", 13, 1, 4, 1 } },
	// MG shot
	{ 3, { "mg_shot.png", 10, 1, 5, 4 } },
	// lightbolt
	{ 4, { "lightbolt.png", 13, 1, 4, 1 } },
	// flame
	{ 5, { "flame.png", 32, 4, 0, 4 } },
	// plasma
	{ 6, { "plasma.png", 12, 1, 4, 1 } },
	// magma
	{ 7, { "magma.png", 26, 1, 4, 4 } },
	// rocket
	{ 8, { "bullet8.png", /*TODO*/32, 2, 4, 4 } },
	// grenade
	{ 9, { "bullet9.png", 32, 1, 1, 1 } },
	// rocket
	{ 10, { "bullet10.png", 7, 1, 4, 1 } },
	//// FreeBullets ////
	// star
	{ 101, { "bossbullet.png", 20, 2, 5, 0 } },
	// spirit bullets
	{ 102, { "spirit_bullets.png", 16, 2, 3, 0 } },
	// missile
	{ 103, { "missile.png", 32, 4, 4, 1 } },
	// lightning
	{ 104, { "lightning_bullet.png", 32, 4, 4, 1 } },
};
