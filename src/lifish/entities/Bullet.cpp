#include <algorithm>
#include "Bullet.hpp"
#include "AxisMoving.hpp"
#include "Animated.hpp"
#include "Sounded.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "ZIndexed.hpp"
#include "game.hpp"
#include "Temporary.hpp"
#include "utils.hpp"
#include "zindex.hpp"
#include "bullet.hpp"

using Game::Bullet;

Bullet::Bullet(const sf::Vector2f& pos, const Game::BulletInfo& _info, const Game::Entity *const source)
	: Game::Entity(pos)
	, info(_info)
	, data(Game::Conf::Bullet::data[info.id])
	, source(source)
{
	addComponent(new Game::Sounded(*this, {
		Game::getAsset("test", std::string("bullet") + Game::to_string(info.id) + std::string("_hit.ogg")),
		Game::getAsset("test", std::string("bullet") + Game::to_string(info.id) + std::string("_shot.ogg"))
	}));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::BULLETS));

	addComponent(new Game::Temporary(*this, [this] () {
		// expire condition
		return dealtDamage
			|| (info.range > 0 && get<Game::Moving>()->getDistTravelled() > info.range)
			|| position.x < 0 || position.x > (Game::TILE_SIZE + 1) * Game::LEVEL_WIDTH 
			|| position.y < 0 || position.y > (Game::TILE_SIZE + 1) * Game::LEVEL_HEIGHT;
	}, [this] () {
		// on kill
		_destroy();
	}, [this] () {
		// is kill in progress
		const auto& animatedSprite = get<Game::Animated>()->getSprite();
		return animatedSprite.isPlaying();
	}));
}

Game::Entity* Bullet::init() {
	Game::Entity::init();
	if (collider != nullptr)
		collider->setForceAck(true);
	return this;
}

void Bullet::update() {
	Game::Entity::update();
	if (collider->isAtLimit())
		get<Game::Killable>()->kill();
}

void Bullet::_destroy() {
	auto animated = get<Game::Animated>();
	auto moving = get<Game::Moving>();
	auto& animatedSprite = animated->getSprite();
	animatedSprite.setLooped(false);
	moving->stop();
	if (data.nDestroyFrames > 0) {
		animatedSprite.stop();
		animated->setAnimation("destroy");
		animatedSprite.play();
		animatedSprite.setPosition(Game::aligned(position));
	}
}
