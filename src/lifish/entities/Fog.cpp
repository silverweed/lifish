#include "Fog.hpp"
#include "Sprite.hpp"
#include "ZIndexed.hpp"
#include "Clock.hpp"
#include "FreeMoving.hpp"
#include "utils.hpp"
#include "Drawable.hpp"
#include "GameCache.hpp"
#include "core.hpp"
#include "game_values.hpp"
#include <SFML/System.hpp>
#include <random>

using Game::Fog;

Fog::Fog(float speed, sf::Time alphaDt) 
	: Game::Entity(sf::Vector2f(0, 0)) 
	, alphaDt(alphaDt)
{
	sprite = addComponent(new Game::Sprite(*this, Game::getAsset("graphics", "fog.png"), 
				sf::IntRect(0, 0, 
					3 * Game::LEVEL_WIDTH * Game::TILE_SIZE, 
					3 * Game::LEVEL_HEIGHT * Game::TILE_SIZE)));
	sprite->getTexture()->setRepeated(true);
	sprite->getTexture()->setSmooth(true);
	addComponent(new Game::Drawable(*this, *sprite));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::FOG));

	// Set a random velocity
	std::uniform_real_distribution<float> dist(-1, 1);
	const sf::Vector2f velocity(dist(Game::rng), dist(Game::rng));
	moving = addComponent(new Game::FreeMoving(*this, speed, velocity));
	clock = addComponent(new Game::Clock(*this));

	// Set the initial position
	position.x = (1 - Game::LEVEL_WIDTH) * Game::TILE_SIZE;
	position.y = (1 - Game::LEVEL_HEIGHT) * Game::TILE_SIZE;
	sprite->getSprite().setColor(sf::Color(255, 255, 255, 255));
}

void Fog::update() {
	Game::Entity::update();
	
	// Fade in/out sprite
	if (clock->getElapsedTime() >= alphaDt) {
		clock->restart();
		auto c = sprite->getSprite().getColor();
		if (fading) {
			c.a -= 1;
			if (c.a < 5) {
				fading = false;
				// Reset the position
				position.x = (1 - Game::LEVEL_WIDTH) * Game::TILE_SIZE;
				position.y = (1 - Game::LEVEL_HEIGHT) * Game::TILE_SIZE;
			}
		} else {
			c.a += 1;
			if (c.a > 200)
				fading = true; 
		}
		sprite->getSprite().setColor(c);
	}
}
