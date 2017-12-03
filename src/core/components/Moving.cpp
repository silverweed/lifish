#include "Moving.hpp"
#include "Clock.hpp"
#include "Collider.hpp"

using lif::Moving;

const sf::Time Moving::MAX_FRAME_TIME = sf::seconds(1 / 30.f);

Moving::Moving(lif::Entity& owner, float speed)
	: lif::Component(owner)
	, originalSpeed(speed)
	, speed(speed)
{
	_declComponent<Moving>();
	frameClock = addComponent<lif::Clock>(*this);
	blockClock = addComponent<lif::Clock>(*this);
}

lif::Entity* Moving::init() {
	lif::Component::init();

	// optional
	collider = owner.get<lif::Collider>();

	return this;
}

void Moving::move() {
	moving = true;
}

void Moving::stop() {
	moving = false;
}

void Moving::block(sf::Time duration) {
	if (duration <= sf::Time::Zero)
		blocked = false;
	else {
		blocked = true;
		blockTime = duration;
		blockClock->restart();
	}
}

void Moving::setDashing(float mult) {
	if (onDashChange)
		onDashChange(dashAmount, mult);
	dashAmount = mult;
}

bool Moving::_collidesWithSolid() const {
	return collider != nullptr && collider->collidesWithSolid();
}

bool Moving::_handleBlock() {
	if (!blocked) return false;
	if (blockClock->getElapsedTime() > blockTime) {
		blocked = false;
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
