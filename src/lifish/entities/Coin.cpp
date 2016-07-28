#include <cmath>
#include <forward_list>
#include "Coin.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"
#include "game.hpp"
#include "GameCache.hpp"
#include "Killable.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
#include "Player.hpp"
#include "Fixed.hpp"
#include "Spawning.hpp"

using Game::Coin;
using Game::TILE_SIZE;

const sf::Time Coin::GRAB_TIME = sf::milliseconds(3000);

Coin::Coin(const sf::Vector2f& pos)
	: Game::Entity(pos)
{
	addComponent(new Game::Fixed(*this));
	addComponent(new Game::Scored(*this, VALUE));
	addComponent(new Game::Sounded(*this, { Game::getAsset("sounds", "coin.ogg") }));
	grabClock = addComponent(new Game::Clock(*this));
	std::string texname = Game::getAsset("graphics", "coin.png");
	animated = addComponent(new Game::Animated(*this, texname));
	Game::cache.loadTexture(texname)->setSmooth(true);
	addComponent(new Game::Drawable(*this, *animated));
	addComponent(new Game::Collider(*this, [this] (Game::Collider& coll) {
		// only collides with player, so no check
		get<Game::Killable>()->kill();			
		get<Game::Scored>()->setTarget(static_cast<const Game::Player&>(coll.getOwner()).getInfo().id);
	}));
	addComponent(new Game::Killable(*this, [this] () {
		// on kill
		_grab();
	}, [this] () {
		// is kill in progress
		return grabbed && grabClock->getElapsedTime() < GRAB_TIME;
	}));

	auto& anim = animated->addAnimation("spin");

	// Coins have 10 sprites
	for (unsigned short i = 0; i < 10; ++i)
		anim.addFrame(sf::IntRect(i * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(anim);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.02));
	animatedSprite.pause();
}

void Coin::_grab() {
	if (!grabbed) {
		grabbed = true;
		grabClock->restart();
		animated->getSprite().play();
	}
}
