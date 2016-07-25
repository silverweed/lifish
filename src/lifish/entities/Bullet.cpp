#include <algorithm>
#include "Bullet.hpp"
#include "AxisMoving.hpp"
#include "Sounded.hpp"
#include "Drawable.hpp"
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

	/*
	switch (direction) {
	case Direction::UP:
		shift.y -= speed;
		break;
	case Direction::LEFT:
		shift.x -= speed;
		break;
	case Direction::DOWN:
		shift.y += speed;
		break;
	case Direction::RIGHT:
		shift.x += speed;
		break;
	case Direction::NONE:
		return;
	}
	*/

	addComponent(new Game::Temporary(*this, [this] () {
		// expire condition
		return position.x < 0 || position.x > Game::TILE_SIZE * Game::LEVEL_WIDTH 
			|| position.y < 0 || position.y > Game::TILE_SIZE * Game::LEVEL_HEIGHT;
	}, [this] () {
		// on kill
		_destroy();
	}, [this] () {
		// is kill in progress
		auto& animatedSprite = get<Game::Animated>()->getSprite();
		return animatedSprite.isPlaying();
	}));
}

//void Bullet::move() {
	//if (direction == Game::Direction::NONE) {
		//animatedSprite.update(frameClock.restart());
		//return;
	//}
	//if (range > 0 && (Game::abs(pos.x - origin.x) > Game::TILE_SIZE * range 
				//|| Game::abs(pos.y - origin.y) > Game::TILE_SIZE * range)) {
		//destroy();
		//return;
	//}

	//sf::Time frameTime = frameClock.restart();

	//if (!colliding) {
		//animatedSprite.move(shift * frameTime.asSeconds());
		//pos = animatedSprite.getPosition();
		//// Ensure we are always aligned at least for one frame for
		//// each tile we step in (this may not be the case if FPS are too low)
		//_ensureAlign();
	//} 
	//animatedSprite.update(frameTime);
//}

//bool Bullet::hits(const sf::Vector2f& epos) const {
	//sf::FloatRect me(pos.x, pos.y, size, size);
	//sf::FloatRect it(epos.x, epos.y, TILE_SIZE, TILE_SIZE);
	//return me.intersects(it);
//}

void Bullet::_destroy() {
	auto animated = get<Game::Animated>();
	auto moving = get<Game::AxisMoving>();
	auto& animatedSprite = animated->getSprite();
	if (nDestroyFrames > 0) {
		switch (moving->getDirection()) {
		case Game::Direction::UP: case Game::Direction::DOWN:
			position.x = Game::aligned(position).x;
			break; 
		default:
			position.y = Game::aligned(position).y;
			break; 
		}
		animated->setAnimation("destroy");
	}
	animatedSprite.setLooped(false);
	moving->stop();
}
