#include "Moving.hpp"

using Game::Moving;

const sf::Time Moving::MAX_FRAME_TIME = sf::seconds(1 / 60.f);

Moving::Moving(Game::Entity *const owner, float speed)
	: Game::Component(owner)
	, speed(speed)
	, originalSpeed(speed)
{
	frameClock = addComponent(new Game::Clock(this));
	// optional
	collider = owner->get<Game::Collider>();
}

void Moving::move() {
	moving = true;
}

void Moving::stop() {
	moving = false;
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
