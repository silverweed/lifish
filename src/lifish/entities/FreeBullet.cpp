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
	addComponent(new lif::FreeMoving(*this, lif::conf::bullet::BASE_SPEED * info.speed,
				sf::Vector2f(std::sin(angle.asRadians()), -std::cos(angle.asRadians()))));
	collider = addComponent(new lif::Collider(*this, [this] (lif::Collider&) {
		// on collision
		auto klb = get<lif::Killable>();
		if (!klb->isKilled()) {
			klb->kill();
		}
	}, lif::c_layers::BOSS_BULLETS, sf::Vector2f(data.size, data.size)));

	position.x += (TILE_SIZE - data.size) / 2;
	position.y += (TILE_SIZE - data.size) / 2;

	auto animated = addComponent(new lif::Animated(*this, lif::getAsset("test", "freebullets.png")));
	addComponent(new lif::Drawable(*this, *animated));
	auto& a_move = animated->addAnimation("move");
	auto& a_destroy = animated->addAnimation("destroy");
	for (unsigned short i = 0; i < data.nMotionFrames; ++i) 
		a_move.addFrame(sf::IntRect(
				i * TILE_SIZE,
				(info.id - 101) * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));
	for (unsigned short i = 0; i < data.nDestroyFrames; ++i)
		a_destroy.addFrame(sf::IntRect(
				(data.nMotionFrames + i) * TILE_SIZE,
				(info.id - 101) * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_move);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.06));
	animatedSprite.play();
}
