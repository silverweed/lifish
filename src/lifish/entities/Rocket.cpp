#include "Rocket.hpp"
#include "Clock.hpp"
#include "Moving.hpp"
#include "conf/bullet.hpp"

using lif::Rocket;

Rocket::Rocket(const sf::Vector2f& pos, lif::Direction dir, const lif::BulletInfo& info,
			const lif::Entity *const source)
	: lif::AxisBullet(pos, dir, info, source)
	, maxSpeed(lif::conf::bullet::BASE_SPEED * info.speed * 20)
{
	accelClock = addComponent<lif::Clock>(*this);
}

void Rocket::update() {
	lif::AxisBullet::update();

	static constexpr auto ACCEL = 350;

	if (moving->getSpeed() < maxSpeed)
		moving->setSpeed(moving->getSpeed() + ACCEL * accelClock->restart().asSeconds(), false);
}
