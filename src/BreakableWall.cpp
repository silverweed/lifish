#include "BreakableWall.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "Killable.hpp"
#include "Lifed.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"

using Game::BreakableWall;
using Game::TILE_SIZE;

BreakableWall::BreakableWall(const sf::Vector2f& pos, const std::string& texture_name, 
		unsigned short life, unsigned int score)
{
	// TODO
	//transparentTo.players = false;
	//transparentTo.enemies = false;
	//transparentTo.bullets = false;
	//transparentTo.explosions = false;
	
	_setupComponents(life, score);
	auto animation = _setupAnimations(texture_name);
	for (unsigned short i = 0; i < 4; ++i)
		animation.addFrame(sf::IntRect(
					i * TILE_SIZE, 
					0,
					TILE_SIZE, 
					TILE_SIZE));
}

BreakableWall::BreakableWall(const sf::Vector2f& pos, 
		const unsigned short id, unsigned short life)
{
	_setupComponents(life, SCORE);
	auto animation = _setupAnimations(Game::getAsset("graphics", "breakable.png"));	
	for (unsigned short i = 0; i < 4; ++i)
		animation.addFrame(sf::IntRect(
					i * TILE_SIZE, 
					(id-1) * TILE_SIZE, 
					TILE_SIZE,
					TILE_SIZE));
}

void BreakableWall::_setupComponents(unsigned short life, unsigned int score) {
	addComponent(new Game::Scored(this, score));
	addComponent(new Game::Lifed(this, life));
	addComponent(new Game::Sounded(this, { Game::getAsset("sounds", "wall_break.ogg") })); 
	addComponent(new Game::Killable(this, [this] (Game::Entity*) {
		// on kill
		get<Game::Animated>()->getSprite().play();
	}, [this] (Game::Entity*) {
		// is kill in progress
		return get<Game::Animated>()->getSprite().isPlaying();
	}));
}

Animation& BreakableWall::_setupAnimations(const std::string& texture_name) {
	
	auto animated = addComponent(new Game::Animated(this, texture_name));
	auto& animation = animated->addAnimation("break", true);
	auto animatedSprite = animated->getSprite();

	animatedSprite.setLooped(false);
	animatedSprite.setFrameTime(sf::seconds(0.2));
	animatedSprite.pause();

	return animation;
}
