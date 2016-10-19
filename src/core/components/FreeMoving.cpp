#include "FreeMoving.hpp"
#include "Clock.hpp"

using Game::FreeMoving;

FreeMoving::FreeMoving(Game::Entity& owner, float speed, sf::Vector2f velocity)
	: Game::Moving(owner, speed)
	, velocity(velocity)
{
	moving = true;
}

void FreeMoving::update() {
	Game::Component::update();
	if (!moving || _handleBlock()) return;

	const auto effSpeed = _effectiveSpeed();

	sf::Vector2f shift(0.f, 0.f);
	sf::Time frameTime = frameClock->restart();

	if (frameTime > MAX_FRAME_TIME)
		frameTime = MAX_FRAME_TIME;
	
	shift += velocity * effSpeed;

	// TODO
	//if (!colliding) {
		owner.setPosition(owner.getPosition() + shift * frameTime.asSeconds());
		distTravelled += speed * frameTime.asSeconds();
	//}
}

