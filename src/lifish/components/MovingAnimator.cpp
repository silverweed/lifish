#include "MovingAnimator.hpp"

using Game::MovingAnimator;

MovingAnimator::MovingAnimator(Game::Entity& owner)
	: Game::Component(owner)
{}

Game::Entity* MovingAnimator::init() {
	moving = owner.get<Game::AxisMoving>();
	if (moving == nullptr)
		throw std::invalid_argument("owner has no Moving!");

	animated = owner.get<Game::Animated>();
	if (animated == nullptr)
		throw std::invalid_argument("owner has no Animated!");
	
	return this;
}

MovingAnimator::MovingAnimator(Game::Entity& owner, Game::AxisMoving *m, Game::Animated *a)
	: Game::Component(owner)
	, moving(m)
	, animated(a)
{
	if (moving == nullptr)
		throw std::invalid_argument("owner has no Moving!");
	if (animated == nullptr)
		throw std::invalid_argument("owner has no Animated!");
}

void MovingAnimator::update() {
	if (!active) return;

	Game::Component::update();

	Game::Direction dir = moving->getDirection();
	auto& sprite = animated->getSprite();

	if (prev == dir) return;

	if (!moving->isMoving()) {
		if (prev != Game::Direction::NONE) {
			animated->setAnimation("walk_down");
			sprite.stop();	
		}
		prev = dir;
		return;
	}

	switch (dir) {
	case Game::Direction::UP:
		animated->setAnimation("walk_up");
		sprite.play();
		break;
	case Game::Direction::DOWN:
		animated->setAnimation("walk_down");
		sprite.play();
		break;
	case Game::Direction::LEFT:
		animated->setAnimation("walk_left");
		sprite.play();
		break;
	case Game::Direction::RIGHT:
		animated->setAnimation("walk_right");
		sprite.play();
		break;
	default:
		break;
	}

	prev = dir;
}
