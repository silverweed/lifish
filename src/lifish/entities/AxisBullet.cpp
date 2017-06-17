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
	// Indices of the line in the spritesheet, depending on directionality
	int animLine = 0;
	switch (dir) {
	case lif::Direction::DOWN:
		animLine = 0;
		position.x += (TILE_SIZE - data.size) / 2;
		position.y += TILE_SIZE;
		break;
	case lif::Direction::UP:
		animLine = !(data.directionality < 4);
		position.x += (TILE_SIZE - data.size) / 2;
		position.y -= data.size;
		break;
	case lif::Direction::RIGHT:
		animLine = data.directionality == 1 ? 0 : data.directionality == 2 ? 1 : 2;
		position.y += (TILE_SIZE - data.size) / 2;
		position.x += TILE_SIZE;
		break;
	case lif::Direction::LEFT:
		animLine = data.directionality - 1;
		position.y += (TILE_SIZE - data.size) / 2;
		position.x -= data.size;
		break;
	default:
		break;
	}

	moving = addComponent<lif::AxisMoving>(*this, lif::conf::bullet::BASE_SPEED * info.speed, dir);
	static_cast<lif::AxisMoving*>(moving)->setEnsureAlign(false);
	static_cast<lif::AxisMoving*>(moving)->setAutoRealign(false);
	auto animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", data.filename));
	addComponent<lif::Drawable>(*this, *animated);

	auto& a_move = animated->addAnimation("move");
	auto& a_destroy = animated->addAnimation("destroy");

	// Since the bullet cannot change direction, we only need to setup these 2 animations.
	// Format for the spritesheet is:
	//	- (directionality + 1) lines, all but the last containing nMotionFrames frames and the
	//	  last one containing nDestroyFrames frames.
	// 	- if directionality == 2, first line is UP/DOWN, second is LEFT/RIGHT
	//	- if == 4, first line is DOWN, second is UP, third is RIGHT and fourth is LEFT
	for (unsigned i = 0; i < data.nMotionFrames; ++i) {
		a_move.addFrame(sf::IntRect(
				i * TILE_SIZE,
				animLine * TILE_SIZE,
				data.size, data.size));
	}

	// destroy animations are non-directional
	for (unsigned i = 0; i < data.nDestroyFrames; ++i) {
		a_destroy.addFrame(sf::IntRect(
				i * TILE_SIZE,
				data.directionality * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));
	}

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_move);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.10));
	animatedSprite.play();
}
