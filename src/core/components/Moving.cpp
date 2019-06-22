#include "Moving.hpp"
#include "Collider.hpp"
#include "Time.hpp"

using lif::Moving;

const sf::Time Moving::MAX_FRAME_TIME = sf::seconds(1 / 30.f);

Moving::Moving(lif::Entity& owner, float speed)
	: lif::Component(owner)
	, originalSpeed(speed)
	, speed(speed)
{
	_declComponent<Moving>();
}

lif::Entity* Moving::init() {
	lif::Component::init();

	// optional
	collider = owner.get<lif::Collider>();

	return this;
}

void Moving::update() {
	lif::Component::update();

	blockT += lif::time.getDelta();
	distTravelledThisFrame = 0;
}

void Moving::move() {
	moving = true;
}

void Moving::stop() {
	moving = false;
}

void Moving::block(sf::Time duration) {
	if (duration > sf::Time::Zero) {
		blockTime = duration;
		blockT = sf::Time::Zero;
	}
}

void Moving::setDashing(float mult) {
	if (onDashChange)
		onDashChange(dashAmount, mult);
	dashAmount = mult;
}

bool Moving::_collidesWithSolid(const sf::Vector2f& direction) const {
	return collider != nullptr && collider->collidesWithSolid(direction);
}

bool Moving::_handleBlock() {
	if (!isBlocked()) return false;
	if (blockT > blockTime) {
		blockTime = sf::Time::Zero;
		return false;
	}
	return true;
}

float Moving::_effectiveSpeed() const {
	return speed + dashAmount * originalSpeed;
}

void Moving::setSpeed(float _speed, bool relativeToOriginal) {
	speed = _speed * (relativeToOriginal ? originalSpeed : 1);
}
