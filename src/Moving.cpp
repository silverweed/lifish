#include "Moving.hpp"

using Game::Moving;

const sf::Time Moving::MAX_FRAME_TIME = sf::seconds(1 / 60.f);

Moving::Moving(Game::Entity *const owner, float speed)
	: Game::Component(owner)
	, speed(speed)
	, originalSpeed(speed)
{
	frameClock = addComponent(new Game::Clock<1>(this));
	// optional
	collider = owner->get<Game::Collider>();
}

void Moving::move() {
	moving = true;
}

void Moving::stop() {
	moving = false;
}

#include <iostream>
bool Moving::_collidesWithSolid() const {
	std::cerr << "Collider = " << collider << std::endl;
	if (collider != nullptr) {
		std::cerr << "Collider->getColliding() = " << collider->getColliding() << std::endl;
		if (collider->getColliding() != nullptr)
			std::cerr << "solid[" << int(collider->getLayer()) << "][" << int(collider->getColliding()->getLayer()) << "] = "
				<< Game::Layers::solid[collider->getLayer()][collider->getColliding()->getLayer()] << std::endl;
	}
	return collider != nullptr && collider->getColliding() != nullptr
		&& Game::Layers::solid[collider->getLayer()][collider->getColliding()->getLayer()];
}
