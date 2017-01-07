#include "AxisBullet.hpp"
#include "AxisMoving.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "Collider.hpp"
#include "Killable.hpp"
#include "game.hpp"
#include "conf/bullet.hpp"

using lif::AxisBullet;
using lif::TILE_SIZE;

AxisBullet::AxisBullet(const sf::Vector2f& pos, lif::Direction dir,
		const lif::BulletInfo& info, const lif::Entity *const source)
	: lif::Bullet(pos, info, source)
{
	unsigned short d = 0;
	switch (dir) {
	case lif::Direction::DOWN:
		d = 0;
		position.x += (TILE_SIZE - data.size) / 2;
		position.y += TILE_SIZE;
		break;
	case lif::Direction::UP:
		d = data.directionality == 4 ? 1 : 0;
		position.x += (TILE_SIZE - data.size) / 2;
		position.y -= data.size;
		break;
	case lif::Direction::RIGHT:
		d = data.directionality == 4 ? 2 : data.directionality == 2 ? 1 : 0;
		position.y += (TILE_SIZE - data.size) / 2;
		position.x += TILE_SIZE;
		break;
	case lif::Direction::LEFT:
		d = data.directionality == 4 ? 3 : data.directionality == 2 ? 1 : 0;
		position.y += (TILE_SIZE - data.size) / 2;
		position.x -= data.size;
		break;
	default:
		break;
	}

	collider = addComponent(new lif::Collider(*this, [this] (lif::Collider& e) {
		// on collision
		// Note: the default layer doesn't automatically destroy a Bullet for
		// practical reasons: it is typically used as a "catch-all" layer, but
		// it should explicitly tell the bullet to selfdestroy if it's the intended
		// behaviour. The bullet otherwise self-destructs as soon as it collides.
		if (&e.getOwner() == this->source || e.getLayer() == lif::Layers::DEFAULT)
			return;
		auto klb = get<lif::Killable>();
		if (!klb->isKilled()) {
			klb->kill();
		}
	}, lif::Layers::ENEMY_BULLETS, sf::Vector2f(data.size, data.size)));
	auto moving = addComponent(new lif::AxisMoving(*this, lif::Conf::Bullet::BASE_SPEED * info.speed, dir));
	moving->setEnsureAlign(false);
	moving->setAutoRealign(false);
	auto animated = addComponent(new lif::Animated(*this, lif::getAsset("test", "axisbullets.png")));
	addComponent(new lif::Drawable(*this, *animated));

	auto& a_move = animated->addAnimation("move");
	auto& a_destroy = animated->addAnimation("destroy");

	// Since the bullet cannot change direction, we only need to setup these 2 animations.
	// The motion animation has up to 8 / directionality frames (but can also have less),
	// while the destroy animation can have from 0 to 5 frames.
	// Format for the spritesheet is:
	// 	- if directionality == 1, [motion frames] [destroy frames]
	//	- if == 2, [up/down frames] [left/right frames] [destroy frames]
	//	- if == 4, [down] [up] [right] [left] [destroy]
	for (unsigned short i = 0; i < data.nMotionFrames && i < 8 / data.directionality; ++i)
		a_move.addFrame(sf::IntRect(
				(data.nMotionFrames * d + i) * TILE_SIZE,
				(info.id - 1) * TILE_SIZE,
				TILE_SIZE,
				TILE_SIZE));

	// destroy animations are non-directional
	for (unsigned short i = 0; i < data.nDestroyFrames && i < 5; ++i)
		a_destroy.addFrame(sf::IntRect(
				(data.nMotionFrames * data.directionality + i) * TILE_SIZE,
				(info.id - 1) * TILE_SIZE,
				TILE_SIZE,
				TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_move);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.10));
	animatedSprite.play();
}
