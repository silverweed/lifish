#include "BulletFactory.hpp"
#include "AxisBullet.hpp"
#include "AxisMoving.hpp"
#include "FreeBullet.hpp"
#include "collision_layers.hpp"
#include "utils.hpp"

#include <unordered_map>

using lif::BulletFactory;

namespace {

// Regular bullets' info
std::map<unsigned, const lif::BulletInfo> bulletsInfo = {
	// infoId, {dataId, damage, speed, range in pixels, collisionLayer}
	/// AxisBullets
	// shot
	{ 1, lif::BulletInfo{ 1, 1, 1.3f, -1 } },
	// fireball
	{ 2, lif::BulletInfo{ 2, 2, 1.15f, -1 } },
	// MG shot
	{ 3, lif::BulletInfo{ 3, 2, 1.3f, -1 } },
	// lightbolt
	{ 4, lif::BulletInfo{ 4, 3, 1.15f, -1 } },
	// flame
	{ 5, lif::BulletInfo{ 5, 3, 0.7f, 4 * lif::TILE_SIZE } },
	// plasma
	{ 6, lif::BulletInfo{ 6, 3, 1.3f, -1 } },
	// magma
	{ 7, lif::BulletInfo{ 7, 4, 1.3f, -1 } },
	// grenade
	{ 9, lif::BulletInfo{ 9, 2, 1.f, 6 * lif::TILE_SIZE } },
	// rocket
	{ 10, lif::BulletInfo{ 10, 1, 0.2f, -1 } },
	/// FreeBullets
	// star
	{ 101, lif::BulletInfo{ 101, 4, 1.1f, -1, lif::c_layers::BOSS_BULLETS } },
};

}

std::unique_ptr<lif::Bullet> BulletFactory::create(unsigned infoId, const sf::Vector2f& pos,
		const sf::Vector2f& target, const lif::Entity *const source)
{
	switch (infoId) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		{
			auto dir = lif::getDirection(lif::tile(pos), lif::tile(target));
			if (dir == lif::Direction::NONE && source != nullptr) {
				const auto moving = source->get<lif::AxisMoving>();
				if (moving != nullptr)
					dir = moving->getDirection();
			}
			return std::unique_ptr<lif::Bullet>(new lif::AxisBullet(pos, dir
				, bulletsInfo.find(infoId)->second, source));
		}
	case 101:
	case 102:
	case 104:
		return std::unique_ptr<lif::Bullet>(new lif::FreeBullet(pos,
			lif::angleBetween(pos, target), bulletsInfo.find(infoId)->second, source));
	default:
		throw std::invalid_argument("No bullet with id " + lif::to_string(infoId) + "!");
	}
}

std::unique_ptr<lif::Bullet> BulletFactory::create(unsigned infoId, const sf::Vector2f& pos,
		lif::Angle angle, const lif::Entity *const source)
{
	return std::unique_ptr<lif::Bullet>(new lif::FreeBullet(pos, angle, bulletsInfo.find(infoId)->second, source));
}

std::unique_ptr<lif::Bullet> BulletFactory::create(unsigned infoId, const sf::Vector2f& pos,
		lif::Direction dir, const lif::Entity *const source)
{
	return std::unique_ptr<lif::Bullet>(new lif::AxisBullet(pos, dir, bulletsInfo.find(infoId)->second, source));
}
