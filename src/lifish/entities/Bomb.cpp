#include "Bomb.hpp"
#include "Sounded.hpp"
#include "Player.hpp"
#include "Drawable.hpp"
#include "Spawning.hpp"
#include "Clock.hpp"
#include "Animated.hpp"
#include "Temporary.hpp"
#include "GameCache.hpp"
#include "Explosion.hpp"
#include "Collider.hpp"
#include "game.hpp"
#include "Fixed.hpp"
#include "LightSource.hpp"

using lif::Bomb;
using lif::TILE_SIZE;
using namespace lif::conf::bomb;

Bomb::Bomb(const sf::Vector2f& pos, const lif::Player& source,
		const sf::Time& _fuseTime, const unsigned short _radius,
		bool isIncendiary)
	: lif::Entity(pos)
	, fuseTime(_fuseTime)
	, radius(_radius)
	, incendiary(isIncendiary)
	, sourcePlayer(source)
{
	addComponent<lif::Fixed>(*this);
	fuseClock = addComponent<lif::Clock>(*this);
	addComponent<lif::Sounded>(*this, lif::Sounded::SoundList {
		std::make_pair("explosion", lif::getAsset("sounds", "explosion.ogg")),
		std::make_pair("fuse", lif::getAsset("sounds", "fuse.ogg"))
	}); 
	killable = addComponent<lif::Temporary>(*this, [this] () {
		// Expire condition
		return fuseClock->getElapsedTime() >= fuseTime;
	}, [this] () {
		// On kill
		exploded = true;
		lif::cache.playSound(get<lif::Sounded>()->getSoundFile("explosion"));
	});
	animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "bomb.png"));
	addComponent<lif::Collider>(*this, [this] (lif::Collider& cld) {
		// On collide
		if (cld.getLayer() == lif::c_layers::EXPLOSIONS && !ignited)
			ignite();
	}, lif::c_layers::BOMBS);
	addComponent<lif::Drawable>(*this, *animated);
	addComponent<lif::Spawning>(*this, [this] () {
		return new lif::Explosion(position, radius, &sourcePlayer, incendiary);
	});
	addComponent<lif::LightSource>(*this, 0);

	auto& a_normal_idle = animated->addAnimation("normal_idle", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)
	});
	animated->addAnimation("normal_exploding", {
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)
	});
	auto& a_fast_idle = animated->addAnimation("fast_idle", {
		sf::IntRect(0, TILE_SIZE, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE)
	});
	animated->addAnimation("fast_exploding", {
		sf::IntRect(TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE)
	});
	auto& animatedSprite = animated->getSprite();

	animatedSprite.setAnimation(fuseTime < DEFAULT_FUSE ? a_fast_idle : a_normal_idle);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.05));
	animatedSprite.play();
}

void Bomb::update() {
	lif::Entity::update();
	if (!switched && fuseTime - fuseClock->getElapsedTime() < sf::milliseconds(2000)
			&& !killable->isKilled())
	{
		animated->setAnimation(fuseTime < DEFAULT_FUSE && !ignited
				? "fast_exploding" : "normal_exploding");
		switched = true;
	}
}

void Bomb::ignite() {
	fuseTime = sf::milliseconds(50); 
	fuseClock->restart();
	ignited = true; 
}
