#include <algorithm>
#include "Bullet.hpp"
#include "AxisMoving.hpp"
#include "Animated.hpp"
#include "Sounded.hpp"
#include "GameCache.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "ZIndexed.hpp"
#include "game.hpp"
#include "Temporary.hpp"
#include "utils.hpp"
#include "conf/zindex.hpp"
#include "conf/bullet.hpp"

using lif::Bullet;

Bullet::Bullet(const sf::Vector2f& pos, const lif::BulletInfo& _info, const lif::Entity *const source)
	: lif::Entity(pos)
	, info(_info)
	, data(lif::Conf::Bullet::data[info.id])
	, source(source)
{
	addComponent(new lif::Sounded(*this, {
		std::make_pair("hit", lif::getAsset("test", std::string("bullet")
					+ lif::to_string(info.id) + std::string("_hit.ogg"))),
		std::make_pair("shot", lif::getAsset("test", std::string("bullet")
					+ lif::to_string(info.id) + std::string("_shot.ogg")))
	}));
	addComponent(new lif::ZIndexed(*this, lif::Conf::ZIndex::BULLETS));

	addComponent(new lif::Temporary(*this, [this] () {
		// expire condition
		return dealtDamage
			|| (info.range > 0 && get<lif::Moving>()->getDistTravelled() > info.range)
			|| position.x < 0 || position.x > (lif::TILE_SIZE + 1) * lif::LEVEL_WIDTH 
			|| position.y < 0 || position.y > (lif::TILE_SIZE + 1) * lif::LEVEL_HEIGHT;
	}, [this] () {
		// on kill
		_destroy();
	}, [this] () {
		// is kill in progress
		const auto& animatedSprite = get<lif::Animated>()->getSprite();
		return animatedSprite.isPlaying();
	}));
}

lif::Entity* Bullet::init() {
	lif::Entity::init();
	if (collider != nullptr)
		collider->setForceAck(true);
	return this;
}

void Bullet::update() {
	lif::Entity::update();
	if (collider->isAtLimit())
		get<lif::Killable>()->kill();
}

void Bullet::_destroy() {
	auto animated = get<lif::Animated>();
	auto moving = get<lif::Moving>();
	auto& animatedSprite = animated->getSprite();
	lif::cache.playSound(get<lif::Sounded>()->getSoundFile("hit"));
	animatedSprite.setLooped(false);
	moving->stop();
	if (data.nDestroyFrames > 0) {
		animatedSprite.stop();
		animated->setAnimation("destroy");
		animatedSprite.play();
		animatedSprite.setPosition(lif::aligned(position));
	}
}
