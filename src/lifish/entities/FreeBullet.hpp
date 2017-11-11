#pragma once

#include "Bullet.hpp"
#include "Angle.hpp"

namespace lif {

/** A Bullet which travels along any angle */
class FreeBullet : public lif::Bullet {
public:
	/** Constructs a Bullet with a source Entity (using that Entity's position),
	 *  traveling at `angle` radians from the vertical axis (CW).
	 */
	explicit FreeBullet(const sf::Vector2f& pos, lif::Angle angle,
			const lif::BulletInfo& info,
			const lif::Entity *const source = nullptr);
};

}
