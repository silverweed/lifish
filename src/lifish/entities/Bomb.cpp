#include "Bomb.hpp"
#include "Animated.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "Explosion.hpp"
#include "Fixed.hpp"
#include "GameCache.hpp"
#include "Player.hpp"
#include "Sounded.hpp"
#include "Spawning.hpp"
#include "Temporary.hpp"
#include "Time.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"

using lif::Bomb;
using lif::TILE_SIZE;
using namespace lif::conf::bomb;

Bomb::Bomb(const sf::Vector2f& pos, const lif::Entity *const source,
		const sf::Time& _fuseTime, const unsigned short _radius)
	: lif::Entity(pos)
	, fuseTime(_fuseTime)
	, radius(_radius)
	, sourceEntity(source)
{
	addComponent<lif::Sounded>(*this,
		lif::sid("explosion"), lif::getAsset("sounds", "explosion.ogg"),
		lif::sid("fuse"), lif::getAsset("sounds", "fuse.ogg")
	);
	killable = addComponent<lif::Temporary>(*this, [this] () {
		// Expire condition
		return fuseT >= fuseTime && isAligned();
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
		return new lif::Explosion(position, radius, sourceEntity, lif::conf::bomb::EXPL_DAMAGE);
	});
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::BOMBS);

	auto& a_normal_idle = animated->addAnimation("normal_idle", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)
	});
	animated->addAnimation("normal_exploding", {
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE)
	});
	auto& animatedSprite = animated->getSprite();

	animatedSprite.setAnimation(a_normal_idle);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.05));
	animatedSprite.play();
}

void Bomb::update() {
	lif::Entity::update();
	fuseT += lif::time.getDelta();
	if (!switched && fuseTime - fuseT < sf::seconds(2)
			&& !killable->isKilled())
	{
		animated->setAnimation("normal_exploding");
		switched = true;
	}
}

void Bomb::ignite() {
	fuseTime = sf::milliseconds(50);
	fuseT = sf::Time::Zero;
	ignited = true;
}

sf::Time Bomb::getCurrentFuse() const {
	return fuseT;
}
