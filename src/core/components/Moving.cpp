#include "Moving.hpp"
#include "Clock.hpp"
#include "Collider.hpp"

using Game::Moving;

const sf::Time Moving::MAX_FRAME_TIME = sf::seconds(1 / 60.f);

Moving::Moving(Game::Entity& owner, float speed)
	: Game::Component(owner)
	, originalSpeed(speed)
	, speed(speed)
{
	frameClock = addComponent(new Game::Clock(*this));
	blockClock = addComponent(new Game::Clock(*this));
}

Game::Entity* Moving::init() {
	Game::Component::init();

	// optional
	collider = owner.get<Game::Collider>();

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
