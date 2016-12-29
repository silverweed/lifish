#include "FreeBullet.hpp"
#include "FreeMoving.hpp"
#include "Attack.hpp"
#include "Killable.hpp"
#include "core.hpp"
#include "collision_layers.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "Animated.hpp"
#include <cmath>

using Game::FreeBullet;
using Game::TILE_SIZE;

FreeBullet::FreeBullet(const sf::Vector2f& pos, const Game::Entity *const source, 
		const double angle, const Game::Attack& attack)
	: Game::Bullet(pos, source, attack)
{
	addComponent(new Game::FreeMoving(*this, attack.speed, sf::Vector2f(std::cos(angle), std::sin(angle))));
	collider = addComponent(new Game::Collider(*this, [this] (Game::Collider&) {
		// on collision
		auto klb = get<Game::Killable>();
		if (!klb->isKilled()) {
			klb->kill();
		}
	}, Game::Layers::BOSS_BULLETS, sf::Vector2i(info.size, info.size)));

	position.x += (TILE_SIZE - info.size) / 2;
	position.y += (TILE_SIZE - info.size) / 2;

	auto animated = addComponent(new Game::Animated(*this, Game::getAsset("test", "freebullets.png")));
	addComponent(new Game::Drawable(*this, *animated));
	auto& a_move = animated->addAnimation("move");
	auto& a_destroy = animated->addAnimation("destroy");
	for (unsigned short i = 0; i < info.nMotionFrames; ++i) 
		a_move.addFrame(sf::IntRect(
				i * TILE_SIZE,
				(attack.id - 101) * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));
	for (unsigned short i = 0; i < info.nDestroyFrames; ++i)
		a_destroy.addFrame(sf::IntRect(
				(info.nMotionFrames + i) * TILE_SIZE,
				(attack.id - 101) * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_move);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.06));
	animatedSprite.play();
}
