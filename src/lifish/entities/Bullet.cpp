#include <algorithm>
#include "Bullet.hpp"
#include "AxisMoving.hpp"
#include "Sounded.hpp"
#include "Drawable.hpp"
#include "ZIndexed.hpp"
#include "game.hpp"
#include "Temporary.hpp"
#include "utils.hpp"

using Game::Bullet;

//Bullet::Bullet(const sf::Vector2f& _pos, const std::string& texture_name) :
	//Game::MovingEntity(_pos, texture_name),
	//Game::Sounded({ Game::getAsset("test", "bossbullet_hit.ogg"),
			//Game::getAsset("test", "bossbullet_shot.ogg") }),
	//size(0),
	//range(-1),
	//damage(0),
	//shift(0.f, 0.f)
//{
	//// TODO
//}

Bullet::Bullet(const sf::Vector2f& pos, const Game::Attack& attack)
	: Game::Bullet(nullptr, attack)
{
	position = pos;
	setOrigin(pos);
}

Bullet::Bullet(const Game::Entity *const source, const Game::Attack& attack)
	: Game::Entity(source != nullptr ? source->getPosition() : sf::Vector2f(0, 0))
	, origin(position)
	, source(source)
	, damage(attack.damage)
	, range(attack.rangeInTiles ? attack.tileRange : attack.pixelRange)
{
	addComponent(new Game::Sounded(*this, {
		Game::getAsset("test", std::string("bullet") + Game::to_string(attack.id) + std::string("_hit.ogg")),
		Game::getAsset("test", std::string("bullet") + Game::to_string(attack.id) + std::string("_shot.ogg"))
	}));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::BULLETS));

	addComponent(new Game::Temporary(*this, [this] () {
		// expire condition
		return dealtDamage
			|| position.x < 0 || position.x > (Game::TILE_SIZE + 1) * Game::LEVEL_WIDTH 
			|| position.y < 0 || position.y > (Game::TILE_SIZE + 1) * Game::LEVEL_HEIGHT
			|| (range > 0 && get<Game::Moving>()->getDistTravelled() > range);
	}, [this] () {
		// on kill
		_destroy();
	}, [this] () {
		// is kill in progress
		auto& animatedSprite = get<Game::Animated>()->getSprite();
		return animatedSprite.isPlaying();
	}));
}

void Bullet::_destroy() {
	auto animated = get<Game::Animated>();
	auto moving = get<Game::AxisMoving>();
	auto& animatedSprite = animated->getSprite();
	animatedSprite.setLooped(false);
	moving->stop();
	if (nDestroyFrames > 0) {
		animatedSprite.stop();
		animated->setAnimation("destroy");
		animatedSprite.play();
		animatedSprite.setPosition(Game::aligned(position));
	}
}
