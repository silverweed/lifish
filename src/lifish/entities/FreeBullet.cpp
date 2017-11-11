#include "FreeBullet.hpp"
#include "FreeMoving.hpp"
#include "Killable.hpp"
#include "conf/bullet.hpp"
#include "core.hpp"
#include "collision_layers.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "Animated.hpp"
#include <cmath>

using lif::FreeBullet;
using lif::TILE_SIZE;

FreeBullet::FreeBullet(const sf::Vector2f& pos, lif::Angle angle,
		const lif::BulletInfo& info, const lif::Entity *const source)
	: lif::Bullet(pos, info, source)
{
	moving = addComponent<lif::FreeMoving>(*this, lif::conf::bullet::BASE_SPEED * info.speed,
				sf::Vector2f(std::sin(angle.asRadians()), -std::cos(angle.asRadians())));
	_setup();

	get<lif::Drawable>()->setRotOrigin(data.size / 2, data.size / 2);
	get<lif::Drawable>()->setRotation(lif::degrees(90) - angle);
}
