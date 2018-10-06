#include "AlienSprite.hpp"
#include "game.hpp"
#include "GameCache.hpp"
#include "MovingAnimator.hpp"
#include "AxisMoving.hpp"
#include "Animated.hpp"
#include "utils.hpp"
#include "Drawable.hpp"

using lif::AlienSprite;
using lif::Direction;
using lif::TILE_SIZE;

AlienSprite::AlienSprite(lif::Entity& owner)
	: lif::Component(owner)
{
	_declComponent<AlienSprite>();
	animated = addComponent<lif::Animated>(owner, lif::getAsset("graphics", "aliensprite.png"));
	addComponent<lif::Drawable>(*this, *animated);

	auto& a_down = animated->addAnimation("walk_down");
	auto& a_up = animated->addAnimation("walk_up");
	auto& a_left = animated->addAnimation("walk_left");
	auto& a_right = animated->addAnimation("walk_right");
	auto& a_death = animated->addAnimation("death");
	animated->addAnimation("idle_down", { sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE) });
	animated->addAnimation("idle_up", { sf::IntRect(4 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE) });
	animated->addAnimation("idle_right", { sf::IntRect(8 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE) });
	animated->addAnimation("idle_left", { sf::IntRect(3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE) });

	a_down.addFrame(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	a_down.addFrame(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	a_down.addFrame(sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	a_down.addFrame(sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	a_up.addFrame(sf::IntRect(4 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	a_up.addFrame(sf::IntRect(5 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	a_up.addFrame(sf::IntRect(6 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	a_up.addFrame(sf::IntRect(7 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	a_right.addFrame(sf::IntRect(8 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	a_right.addFrame(sf::IntRect(0, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_right.addFrame(sf::IntRect(TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_right.addFrame(sf::IntRect(2 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_left.addFrame(sf::IntRect(3 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_left.addFrame(sf::IntRect(4 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_left.addFrame(sf::IntRect(5 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_left.addFrame(sf::IntRect(6 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_death.addFrame(sf::IntRect(7 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_death.addFrame(sf::IntRect(8 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_down);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.12));
	animatedSprite.play();
}

lif::Entity* AlienSprite::init() {
	lif::Component::init();
	movingAnimator = addComponent<lif::MovingAnimator>(*this, owner.get<lif::AxisMoving>(), animated);
	return this;
}
