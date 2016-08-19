#include "Moving.hpp"

using Game::Moving;

const sf::Time Moving::MAX_FRAME_TIME = sf::seconds(1 / 60.f);

Moving::Moving(Game::Entity& owner, float speed)
	: Game::Component(owner)
	, speed(speed)
	, originalSpeed(speed)
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

void Moving::setDashing(bool d, float mult) {
	if (!d) {
		dashing = false;
		speed = originalSpeed;
	} else if (!dashing) {
		dashing = true;
		speed *= mult;
	}
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
