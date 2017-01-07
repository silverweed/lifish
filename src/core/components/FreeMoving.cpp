#include "FreeMoving.hpp"
#include "Clock.hpp"
#include "utils.hpp"

using lif::FreeMoving;

FreeMoving::FreeMoving(lif::Entity& owner, float speed, sf::Vector2f velocity)
	: lif::Moving(owner, speed)
{
	moving = true;
	setVelocity(velocity);
}

void FreeMoving::update() {
	lif::Component::update();
	if (!moving || _handleBlock()) return;

	const auto effSpeed = _effectiveSpeed();

	sf::Vector2f shift(0.f, 0.f);
	sf::Time frameTime = frameClock->restart();

	if (frameTime > MAX_FRAME_TIME)
		frameTime = MAX_FRAME_TIME;
	
	shift += velocity * effSpeed;

	// TODO
	if (!_collidesWithSolid()) {
		owner.setPosition(owner.getPosition() + shift * frameTime.asSeconds());
		distTravelled += speed * frameTime.asSeconds();
	}
}

void FreeMoving::setVelocity(const sf::Vector2f& vel) {
	velocity = lif::normalized(vel);
}
