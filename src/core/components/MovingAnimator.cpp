#include "MovingAnimator.hpp"
#include "AxisMoving.hpp"
#include "Animated.hpp"

using lif::MovingAnimator;

MovingAnimator::MovingAnimator(lif::Entity& owner)
	: lif::Component(owner)
{
	_declComponent<MovingAnimator>();
}

MovingAnimator::MovingAnimator(lif::Entity& owner, lif::AxisMoving *m, lif::Animated *a)
	: lif::Component(owner)
	, moving(m)
	, animated(a)
{
	_declComponent<MovingAnimator>();
	if (moving == nullptr)
		throw std::invalid_argument("owner has no Moving!");
	if (animated == nullptr)
		throw std::invalid_argument("owner has no Animated!");
}

lif::Entity* MovingAnimator::init() {
	moving = owner.get<lif::AxisMoving>();
	if (moving == nullptr)
		throw std::invalid_argument("owner has no Moving!");

	animated = owner.get<lif::Animated>();
	if (animated == nullptr)
		throw std::invalid_argument("owner has no Animated!");

	return this;
}

void MovingAnimator::update() {
	lif::Component::update();

	lif::Direction dir = moving->getDirection();
	auto& sprite = animated->getSprite();

	if (prev == dir) return;

	if (!moving->isMoving()) {
		if (prev != lif::Direction::NONE) {
			animated->setAnimation(sid("idle_" + lif::directionToString(prev)));
			sprite.play();
		}
		prev = dir;
		return;
	}

	switch (dir) {
	case lif::Direction::UP:
		animated->setAnimation("walk_up");
		sprite.play();
		break;
	case lif::Direction::DOWN:
		animated->setAnimation("walk_down");
		sprite.play();
		break;
	case lif::Direction::LEFT:
		animated->setAnimation("walk_left");
		sprite.play();
		break;
	case lif::Direction::RIGHT:
		animated->setAnimation("walk_right");
		sprite.play();
		break;
	default:
		break;
	}

	prev = dir;
}
