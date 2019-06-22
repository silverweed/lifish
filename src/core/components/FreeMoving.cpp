#include "FreeMoving.hpp"
#include "Time.hpp"
#include "utils.hpp"

using lif::FreeMoving;

FreeMoving::FreeMoving(lif::Entity& owner, float speed, sf::Vector2f velocity)
	: lif::Moving(owner, speed)
{
	_declComponent<FreeMoving>();
	moving = true;
	setVelocity(velocity);
}

void FreeMoving::update() {
	lif::Moving::update();
	if (!moving || _handleBlock()) return;

	const auto effSpeed = _effectiveSpeed();

	sf::Vector2f shift(0.f, 0.f);
	const auto frameTime = lif::time.getDelta();

	shift += velocity * effSpeed;

	const auto prevPos = owner.getPosition();

	if (!_collidesWithSolid(velocity)) {
		owner.setPosition(owner.getPosition() + shift * frameTime.asSeconds());
		distTravelled += speed * frameTime.asSeconds();
	}

	distTravelledThisFrame = lif::length(owner.getPosition() - prevPos);
}

void FreeMoving::setVelocity(const sf::Vector2f& vel) {
	velocity = lif::normalized(vel);
}
