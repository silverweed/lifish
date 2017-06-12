#include "FreeBullet.hpp"
#include "FreeMoving.hpp"
#include "Killable.hpp"
#include "conf/bullet.hpp"
#include "core.hpp"
#include "collision_layers.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "Animated.hpp"
#include <cmath>

using lif::FreeBullet;
using lif::TILE_SIZE;

FreeBullet::FreeBullet(const sf::Vector2f& pos, lif::Angle angle,
		const lif::BulletInfo& info, const lif::Entity *const source)
	: lif::Bullet(pos, info, source)
{
	moving = addComponent<lif::FreeMoving>(*this, lif::conf::bullet::BASE_SPEED * info.speed,
				sf::Vector2f(std::sin(angle.asRadians()), -std::cos(angle.asRadians())));

	position.x += (TILE_SIZE - data.size) / 2;
	position.y += (TILE_SIZE - data.size) / 2;

	auto animated = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "freebullets.png"));
	addComponent<lif::Drawable>(*this, *animated);
	
	auto& a_move = animated->addAnimation("move");
	auto& a_destroy = animated->addAnimation("destroy");

	for (unsigned i = 0; i < data.nMotionFrames; ++i)
		a_move.addFrame(sf::IntRect(
				i * TILE_SIZE,
				(info.dataId - 101) * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));
	for (unsigned i = 0; i < data.nDestroyFrames; ++i)
		a_destroy.addFrame(sf::IntRect(
				(data.nMotionFrames + i) * TILE_SIZE,
				(info.dataId - 101) * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_move);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.06));
	animatedSprite.play();
}
