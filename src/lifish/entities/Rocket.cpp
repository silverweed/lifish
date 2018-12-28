#include "Rocket.hpp"
#include "Moving.hpp"
#include "Time.hpp"
#include "conf/bullet.hpp"

using lif::Rocket;

Rocket::Rocket(const sf::Vector2f& pos, lif::Direction dir, const lif::BulletInfo& info,
			const lif::Entity *const source)
	: lif::AxisBullet(pos, dir, info, source)
	, maxSpeed(lif::conf::bullet::BASE_SPEED * info.speed * 20)
{}

void Rocket::update() {
	lif::AxisBullet::update();

	static constexpr auto ACCEL = 350;

	if (moving->getSpeed() < maxSpeed)
		moving->setSpeed(moving->getSpeed() + ACCEL * lif::time.getDelta().asSeconds(), false);
}
