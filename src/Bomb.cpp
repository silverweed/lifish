#include "Bomb.hpp"
#include "Animated.hpp"
#include "Sounded.hpp"
#include "Player.hpp"
#include "Game.hpp"

using Game::Bomb;
using Game::TILE_SIZE;

Bomb::Bomb(const sf::Vector2f& pos, const Game::Player *const source, 
		const unsigned short _fuseTime, const unsigned short _radius)
	: pos(pos)
	, fuseTime(_fuseTime)
	, radius(_radius)
	, sourcePlayer(source)
{
	fuseClock = addComponent(new Game::Clock<1>(this));
	addComponent(new Game::Sounded(this, { Game::getAsset("sounds", "fuse.ogg") })); 
	auto animated = addComponent(new Game::Animated(this, Game::getAsset("graphics", "bomb.png")));
	auto& a_normal_idle = animated->addAnimation("normal_idle");
	auto& a_normal_exploding = animated->addAnimation("normal_exploding");
	auto& a_fast_idle = animated->addAnimation("fast_idle");
	auto& a_fast_exploding = animated->addAnimation("fast_exploding");
	auto& animatedSprite = animated->getSprite();

	a_normal_idle.addFrame(sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE));
	a_normal_idle.addFrame(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	a_normal_exploding.addFrame(sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	a_normal_exploding.addFrame(sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));
	a_fast_idle.addFrame(sf::IntRect(0, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_fast_idle.addFrame(sf::IntRect(TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_fast_exploding.addFrame(sf::IntRect(TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	a_fast_exploding.addFrame(sf::IntRect(2 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE));
	animatedSprite.setAnimation(fuseTime < DEFAULT_FUSE ? a_fast_idle : a_normal_idle);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.05));
	animatedSprite.play();
}

void Bomb::update() {
	if (!switched && fuseTime - fuseClock->getElapsedTime().asMilliseconds() < 2000 && !isExploded()) {
		auto& anim = get<Animated>()->getAnimation(fuseTime < DEFAULT_FUSE 
				? "fast_exploding" : "normal_exploding");
		animatedSprite.play(anim);
		switched = true;
	}
	animatedSprite.update(frameClock.restart());
}

void Bomb::setExploding() {
	fuseTime = 50; 
	fuseClock.restart();
	ignited = true; 
}
