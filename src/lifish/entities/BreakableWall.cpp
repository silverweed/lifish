#include "BreakableWall.hpp"
#include "game.hpp"
#include "GameCache.hpp"
#include "Spawning.hpp"
#include "Animated.hpp"
#include "Bonus.hpp"
#include "Animation.hpp"
#include "Killable.hpp"
#include "Collider.hpp"
#include "Lifed.hpp"
#include "ZIndexed.hpp"
#include "Scored.hpp"
#include "Player.hpp"
#include "Sounded.hpp"
#include "Explosion.hpp"
#include "Drawable.hpp"
#include "Fixed.hpp"
#include "utils.hpp"
#include "collision_functions.hpp"
#include "conf/wall.hpp"
#include "conf/bonus.hpp"
#include "conf/zindex.hpp"

using lif::BreakableWall;
using lif::TILE_SIZE;

BreakableWall::BreakableWall(const sf::Vector2f& pos, const std::string& texture_name,
		unsigned life, unsigned int score)
	: lif::Entity(pos)
{
	_setupComponents(life, score);
	auto& animation = _setupAnimations(texture_name);
	for (unsigned i = 0; i < 4; ++i)
		animation.addFrame(sf::IntRect(
					i * TILE_SIZE,
					0,
					TILE_SIZE,
					TILE_SIZE));

	animated->setAnimation(animation);
}

BreakableWall::BreakableWall(const sf::Vector2f& pos,
		const unsigned short id, unsigned life)
	: lif::Entity(pos)
{
	_setupComponents(life, lif::conf::wall::breakable::VALUE);
	auto& animation = _setupAnimations(lif::getAsset("graphics", "breakable.png"));
	for (unsigned i = 0; i < 4; ++i)
		animation.addFrame(sf::IntRect(
					i * TILE_SIZE,
					(id-1) * TILE_SIZE,
					TILE_SIZE,
					TILE_SIZE));
	animated->setAnimation(animation);
}

void BreakableWall::_setupComponents(unsigned life, unsigned int score) {
	addComponent<lif::Fixed>(*this);
	addComponent<lif::Scored>(*this, score);
	addComponent<lif::Lifed>(*this, life);
	addComponent<lif::Sounded>(*this,
		lif::sid("death"), lif::getAsset("sounds", "wall_break.ogg")
	);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::WALLS);
	addComponent<lif::Killable>(*this, [this] () {
		// on kill
		animated->getSprite().play();
		lif::cache.playSound(get<lif::Sounded>()->getSoundFile("death"));
	}, [this] () {
		// is kill in progress
		return animated->getSprite().isPlaying();
	});
	addComponent<lif::Collider>(*this, lif::hurtByExplosions(*this, lif::CFO_ONLY_ADJACENT),
				lif::c_layers::BREAKABLES);
	// Spawn bonus on death
	addComponent<lif::Spawning>(*this, [this] () {
		// spawn function
		return _spawnBonus();
	});
}

Animation& BreakableWall::_setupAnimations(const std::string& texture_name) {
	animated = addComponent<lif::Animated>(*this, texture_name);
	addComponent<lif::Drawable>(*this, *animated);
	auto& animation = animated->addAnimation("break");
	auto& animatedSprite = animated->getSprite();

	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.2));
	animatedSprite.pause();

	return animation;
}

lif::Entity* BreakableWall::_spawnBonus() {
	const auto bonus_type = lif::conf::bonus::distribution(lif::rng);
	if (bonus_type < lif::conf::bonus::N_BONUS_TYPES)
		return new lif::Bonus(position, static_cast<lif::BonusType>(bonus_type));

	return nullptr;
}
