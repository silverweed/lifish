#include "BreakableWall.hpp"
#include "game.hpp"
#include "GameCache.hpp"
#include "Lifed.hpp"
#include "ZIndexed.hpp"
#include "Scored.hpp"
#include "Player.hpp"
#include "Sounded.hpp"
#include "Explosion.hpp"
#include "Drawable.hpp"
#include "Fixed.hpp"
#include "utils.hpp"

#include <iostream>
using Game::BreakableWall;
using Game::TILE_SIZE;

BreakableWall::BreakableWall(const sf::Vector2f& pos, const std::string& texture_name, 
		unsigned short life, unsigned int score)
	: Game::Entity(pos)
{
	_setupComponents(life, score);
	auto& animation = _setupAnimations(texture_name);
	for (unsigned short i = 0; i < 4; ++i)
		animation.addFrame(sf::IntRect(
					i * TILE_SIZE, 
					0,
					TILE_SIZE, 
					TILE_SIZE));

	animated->setAnimation(animation);
}

BreakableWall::BreakableWall(const sf::Vector2f& pos, 
		const unsigned short id, unsigned short life)
	: Game::Entity(pos)
{
	_setupComponents(life, SCORE);
	auto& animation = _setupAnimations(Game::getAsset("graphics", "breakable.png"));	
	for (unsigned short i = 0; i < 4; ++i)
		animation.addFrame(sf::IntRect(
					i * TILE_SIZE, 
					(id-1) * TILE_SIZE, 
					TILE_SIZE,
					TILE_SIZE));
	animated->setAnimation(animation);
}

void BreakableWall::_setupComponents(unsigned short life, unsigned int score) {
	addComponent(new Game::Fixed(*this));
	addComponent(new Game::Scored(*this, score));
	addComponent(new Game::Lifed(*this, life));
	addComponent(new Game::Sounded(*this, { Game::getAsset("sounds", "wall_break.ogg") })); 
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::WALLS));
	addComponent(new Game::Collider(*this, [this] (Game::Collider& cld) { 
		// on collision
		_checkCollision(cld); 
	}, Game::Layers::WALLS));
	killable = addComponent(new Game::Killable(*this, [this] () {
		// on kill
		animated->getSprite().play();
	}, [this] () {
		// is kill in progress
		return animated->getSprite().isPlaying();
	}));
}

Animation& BreakableWall::_setupAnimations(const std::string& texture_name) {
	animated = addComponent(new Game::Animated(*this, texture_name));
	addComponent(new Game::Drawable(*this, *animated));
	auto& animation = animated->addAnimation("break");
	auto& animatedSprite = animated->getSprite();

	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.2));
	animatedSprite.pause();

	return animation;
}

void BreakableWall::_checkCollision(Game::Collider& cld) {
	std::cerr << "exploding wall\n";
	if (cld.getLayer() != Game::Layers::EXPLOSIONS || killable->isKilled()) return;
	const auto etile = Game::tile(cld.getOwner().getPosition());
	const auto mtile = Game::tile(position);
	if (Game::manhattanDistance(etile, mtile) == 1) {
		killable->kill();
		get<Game::Scored>()->setTarget(static_cast<const Game::Explosion&>(
					cld.getOwner()).getSourcePlayer()->getInfo().id);
	}
}
