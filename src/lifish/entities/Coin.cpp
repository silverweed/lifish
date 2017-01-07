#include "Coin.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"
#include "game.hpp"
#include "GameCache.hpp"
#include "Killable.hpp"
#include "Drawable.hpp"
#include "Clock.hpp"
#include "Animated.hpp"
#include "Grabbable.hpp"
#include "Collider.hpp"
#include "Player.hpp"
#include "Fixed.hpp"

using lif::Coin;
using lif::TILE_SIZE;

const sf::Time Coin::GRAB_TIME = sf::milliseconds(3000);

Coin::Coin(const sf::Vector2f& pos)
	: lif::Entity(pos)
{
	addComponent(new lif::Fixed(*this));
	addComponent(new lif::Scored(*this, VALUE));
	addComponent(new lif::Sounded(*this, { std::make_pair("grab", lif::getAsset("sounds", "coin.ogg")) }));
	grabClock = addComponent(new lif::Clock(*this));
	std::string texname = lif::getAsset("graphics", "coin.png");
	animated = addComponent(new lif::Animated(*this, texname));
	lif::cache.loadTexture(texname)->setSmooth(true);
	addComponent(new lif::Drawable(*this, *animated));
	grabbable = addComponent(new lif::Grabbable(*this));
	addComponent(new lif::Collider(*this, [this] (lif::Collider& coll) {
		if (coll.getLayer() != lif::c_layers::PLAYERS || grabbable->isGrabbed()) 
			return;
		// only collides with player, so no further check
		get<lif::Killable>()->kill();			
		get<lif::Scored>()->setTarget(static_cast<const lif::Player&>(coll.getOwner()).getInfo().id);
		lif::cache.playSound(get<lif::Sounded>()->getSoundFile("grab"));
	}, lif::c_layers::GRABBABLE));
	addComponent(new lif::Killable(*this, [this] () {
		// on kill
		_grab();
	}, [this] () {
		// is kill in progress
		return grabbable->isGrabbed() && grabClock->getElapsedTime() < GRAB_TIME;
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
	if (!grabbable->isGrabbed()) {
		grabbable->grab();
		grabClock->restart();
		animated->getSprite().play();
	}
}
