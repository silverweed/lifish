#pragma once

#include <memory>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>
#include "Direction.hpp"
#include "Angle.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"
#include "conf/bullet.hpp"

namespace lif {

class Entity;
class Bullet;

/**
 * Utility class used to build bullets from their infoId.
 * An infoId entirely defines a bullet, as it corresponds to a particular
 * instance of a BulletInfo.
 * NOTE: a bullet's infoId should not be confused with bullet's dataId, which
 * is internally used to identify the spritesheet data. Using two different
 * ids allows to treat graphics and behaviour separately.
 */
class BulletFactory final : private sf::NonCopyable {
public:
	static std::unique_ptr<lif::AxisBullet> create(unsigned infoId, const sf::Vector2f& pos,
			lif::Direction dir, const lif::Entity *const source = nullptr);

	static std::unique_ptr<lif::FreeBullet> create(unsigned infoId, const sf::Vector2f& pos,
			lif::Angle angle, const lif::Entity *const source = nullptr);

};

}
