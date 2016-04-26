#include "Moving.hpp"

using Game::Moving;

const sf::Time Moving::MAX_FRAME_TIME = sf::seconds(1 / 60.f);

Moving::Moving(Game::Entity *const owner, float speed)
	: Game::Component(owner)
	, speed(speed)
{
	frameClock = addComponent(new Game::Clock<1>(this));
}

void Moving::move() {
	moving = true;
}

void Moving::stop() {
	moving = false;
}
