#include "Bullet.hpp"
#include "Animated.hpp"
#include "AxisMoving.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "GameCache.hpp"
#include "Sounded.hpp"
#include "Temporary.hpp"
#include "ZIndexed.hpp"
#include "conf/bullet.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"
#include "utils.hpp"
#include <algorithm>

using lif::Bullet;
using namespace std::literals::string_literals;

Bullet::Bullet(const sf::Vector2f& pos, const lif::BulletInfo& _info, const lif::Entity *const source)
	: lif::Entity(pos)
	, info(_info)
	, data(lif::conf::bullet::data[info.dataId])
	, source(source)
	, speed(_info.speed)
{
	addComponent<lif::Sounded>(*this,
		lif::sid("hit"), lif::getAsset("sounds", "bullet"s + lif::to_string(info.dataId) + "_hit.ogg"s),
		lif::sid("shot"), lif::getAsset("sounds", "bullet"s + lif::to_string(info.dataId) + "_shot.ogg"s)
	);
	addComponent<lif::ZIndexed>(*this, info.dataId > 100
			? lif::conf::zindex::BOSS_BULLETS :lif::conf::zindex::BULLETS);
	collider = addComponent<lif::Collider>(*this, [this] (lif::Collider& e) {
		// on collision
		// Note: the default layer doesn't automatically destroy a Bullet for
		// practical reasons: it is typically used as a "catch-all" layer, but
		// it should explicitly tell the bullet to selfdestroy if it's the intended
		// behaviour. The bullet otherwise self-destructs as soon as it collides.
		if (&e.getOwner() == this->source || e.getLayer() == lif::c_layers::DEFAULT)
			return;

		auto klb = get<lif::Killable>();
		if (!klb->isKilled()) {
			klb->kill();
		}
	}, info.cLayer, sf::Vector2f(data.size, data.size));

	addComponent<lif::Temporary>(*this, [this] () {
		// expire condition
		return dealtDamage
			|| (info.range > 0 && get<lif::Moving>()->getDistTravelled() > info.range)
			|| collider->isAtLimit();
	}, [this] () {
		// on kill
		_destroy();
	}, [this] () {
		// is kill in progress
		const auto& animatedSprite = get<lif::Animated>()->getSprite();
		return animatedSprite.isPlaying();
	});
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
		position.x -= (TILE_SIZE - data.size) / 2;
		position.y -= (TILE_SIZE - data.size) / 2;
	}
}

void Bullet::_setup() {
	position.x += (TILE_SIZE - data.size) / 2;
	position.y += (TILE_SIZE - data.size) / 2;

	auto animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", data.filename));
	addComponent<lif::Drawable>(*this, *animated);

	auto& a_move = animated->addAnimation("move");
	auto& a_destroy = animated->addAnimation("destroy");

	// Spritesheet format of a FreeBullet is just 1 line containing all motion frames
	// followed by all destroy frames.
	for (unsigned i = 0; i < data.nMotionFrames; ++i)
		a_move.addFrame(sf::IntRect(
				i * TILE_SIZE,
				0,
				data.size, data.size));

	for (unsigned i = 0; i < data.nDestroyFrames; ++i)
		a_destroy.addFrame(sf::IntRect(
				(data.nMotionFrames + i) * TILE_SIZE,
				0,
				TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_move);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.06));
	animatedSprite.play();
}
