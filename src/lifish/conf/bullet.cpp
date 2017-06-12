#include "bullet.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"
#include "core.hpp"

std::map<unsigned, const lif::BulletData> lif::conf::bullet::data = {
	//// AxisBullets ////
	// dataId, {size, motionFrames, destroyFrames, directionality}
	// shot
	{ 1, { 7, 1, 4, 1 } },
	// fireball
	{ 2, { 13, 1, 4, 1 } },
	// MG shot
	{ 3, { 10, 1, 5, 4 } },
	// lightbolt
	{ 4, { 13, 1, 4, 1 } },
	// flame
	{ 5, { 32, 4, 0, 1 } },
	// plasma
	{ 6, { 12, 1, 4, 1 } },
	// magma
	{ 7, { 26, 1, 4, 4 } },
	// rocket
	{ 8, { /*TODO*/32, 2, 4, 4 } },
	//// FreeBullets ////
	// star
	{ 101, { 20, 2, 5, 0 } },
	// spirit bullets
	{ 102, { 16, 2, 3, 0 } }
};
