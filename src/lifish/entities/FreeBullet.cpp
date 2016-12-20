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

#include <iostream>

using Game::FreeBullet;
using Game::TILE_SIZE;

FreeBullet::FreeBullet(const sf::Vector2f& pos, const Game::Entity *const source, 
		const double angle, const Game::Attack& attack)
	: Game::Bullet(pos, source, attack)
{
	FreeBulletPresets::setup(*this, attack.id);

	addComponent(new Game::FreeMoving(*this, attack.speed, sf::Vector2f(std::cos(angle), std::sin(angle))));
	collider = addComponent(new Game::Collider(*this, [this] (Game::Collider& cld) {
		// on collision
		std::cerr << "colliding with " << cld.getLayer() << std::endl;
		auto klb = get<Game::Killable>();
		if (!klb->isKilled()) {
			klb->kill();
		}
	}, Game::Layers::BOSS_BULLETS, sf::Vector2i(size, size)));

	position.x += (TILE_SIZE - size) / 2;
	position.y += (TILE_SIZE - size) / 2;

	auto animated = addComponent(new Game::Animated(*this, Game::getAsset("test", "bossbullet.png")));
	addComponent(new Game::Drawable(*this, *animated));
	auto& a_move = animated->addAnimation("move");
	auto& a_destroy = animated->addAnimation("destroy");
	for (unsigned short i = 0; i < nMotionFrames; ++i) 
		a_move.addFrame(sf::IntRect(
				i * TILE_SIZE,
				(attack.id - 101) * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));
	for (unsigned short i = 0; i < nDestroyFrames; ++i)
		a_destroy.addFrame(sf::IntRect(
				(nMotionFrames + i) * TILE_SIZE,
				(attack.id - 101) * TILE_SIZE,
				TILE_SIZE, TILE_SIZE));

	auto& animatedSprite = animated->getSprite();
	animatedSprite.setAnimation(a_move);
	animatedSprite.setLooped(true);
	animatedSprite.setFrameTime(sf::seconds(0.06));
	animatedSprite.play();
}

void Game::FreeBulletPresets::setup(Game::FreeBullet& b, unsigned short id) {
	switch (id) {
	case 101:
		// shot
		b.nMotionFrames = 2;
		b.nDestroyFrames = 5;
		b.size = 20;
		break;
	default:
		break;
	}
}
