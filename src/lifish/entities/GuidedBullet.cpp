#include "GuidedBullet.hpp"
#include "game.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"

using lif::GuidedBullet;

GuidedBullet::GuidedBullet(const sf::Vector2f& start, const sf::Vector2f& end, sf::Time timeTaken,
			const lif::BulletInfo& info,
			const lif::Entity *const source,
			std::initializer_list<lif::GuidedMoving::ModFunc> modfuncs)
	: lif::Bullet(start, info, source)
{
	addComponent<lif::GuidedMoving>(*this, start, end, timeTaken, modfuncs);

	position.x += (TILE_SIZE - data.size) / 2;
	position.y += (TILE_SIZE - data.size) / 2;

	auto animated = addComponent<lif::Animated>(*this, lif::getAsset("test", "freebullets.png"));
	addComponent<lif::Drawable>(*this, *animated);
	
	auto& a_move = animated->addAnimation("move");
	auto& a_destroy = animated->addAnimation("destroy");

	for (unsigned i = 0; i < data.nMotionFrames; ++i) 
		a_move.addFrame(sf::IntRect(
				i * TILE_SIZE,
				(info.id - 101) * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));
	for (unsigned i = 0; i < data.nDestroyFrames; ++i)
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
