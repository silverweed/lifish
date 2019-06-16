#include "Coin.hpp"
#include "Animated.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "Fixed.hpp"
#include "GameCache.hpp"
#include "Grabbable.hpp"
#include "Killable.hpp"
#include "Player.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"
#include "Time.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"

using lif::Coin;
using lif::TILE_SIZE;

const sf::Time GRAB_TIME = sf::milliseconds(3000);
constexpr static unsigned int VALUE = 150;

Coin::Coin(const sf::Vector2f& pos)
	: lif::Entity(pos)
{
	addComponent<lif::Fixed>(*this);
	addComponent<lif::Scored>(*this, VALUE);
	addComponent<lif::Sounded>(*this,
		lif::sid("grab"), lif::getAsset("sounds", "coin.ogg")
	);
	std::string texname = lif::getAsset("graphics", "coin.png");
	animated = addComponent<lif::Animated>(*this, texname);
	lif::cache.loadTexture(texname)->setSmooth(true);
	addComponent<lif::Drawable>(*this, *animated);
	grabbable = addComponent<lif::Grabbable>(*this);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::COINS);
	addComponent<lif::Collider>(*this, [this] (lif::Collider& coll) {
		if (coll.getLayer() != lif::c_layers::PLAYERS || grabbable->isGrabbed())
			return;
		// only collides with player, so no further check
		get<lif::Killable>()->kill();
		get<lif::Scored>()->setTarget(static_cast<const lif::Player&>(coll.getOwner()).getInfo().id);
		lif::cache.playSound(get<lif::Sounded>()->getSoundFile("grab"));
	}, lif::c_layers::GRABBABLE);
	addComponent<lif::Killable>(*this, [this] () {
		// on kill
		_grab();
	}, [this] () {
		// is kill in progress
		return grabbable->isGrabbed() && grabT < GRAB_TIME;
	});

	auto& anim = animated->addAnimation("spin");

	// Coins have 10 sprites
	for (unsigned i = 0; i < 10; ++i)
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
		grabT = sf::Time::Zero;
		animated->getSprite().play();
	}
}

void Coin::update() {
	lif::Entity::update();

	grabT += lif::time.getDelta();
}
