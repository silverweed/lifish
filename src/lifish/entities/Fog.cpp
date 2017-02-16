#include "Fog.hpp"
#include "Sprite.hpp"
#include "ZIndexed.hpp"
#include "Clock.hpp"
#include "FreeMoving.hpp"
#include "utils.hpp"
#include "Drawable.hpp"
#include "GameCache.hpp"
#include "core.hpp"
#include "conf/zindex.hpp"
#include <SFML/System.hpp>
#include <random>

using lif::Fog;

Fog::Fog(float speed, sf::Time alphaDt) 
	: lif::Entity(sf::Vector2f(0, 0)) 
	, alphaDt(alphaDt)
{
	sprite = addComponent(std::make_shared<lif::Sprite>(*this, lif::getAsset("graphics", "fog.png"), 
				sf::IntRect(0, 0, 
					3 * lif::LEVEL_WIDTH * lif::TILE_SIZE, 
					3 * lif::LEVEL_HEIGHT * lif::TILE_SIZE)));
	sprite->getTexture()->setRepeated(true);
	sprite->getTexture()->setSmooth(true);
	addComponent(std::make_shared<lif::Drawable>(*this, *sprite));
	addComponent(std::make_shared<lif::ZIndexed>(*this, lif::conf::zindex::FOG));

	// Set a random velocity
	std::uniform_real_distribution<float> dist(-1, 1);
	const sf::Vector2f velocity(dist(lif::rng), dist(lif::rng));
	moving = addComponent(std::make_shared<lif::FreeMoving>(*this, speed, velocity));
	clock = addComponent(std::make_shared<lif::Clock>(*this));

	// Set the initial position
	position.x = (1 - lif::LEVEL_WIDTH) * lif::TILE_SIZE;
	position.y = (1 - lif::LEVEL_HEIGHT) * lif::TILE_SIZE;
	sprite->getSprite().setColor(sf::Color(255, 255, 255, 255));
}

void Fog::update() {
	lif::Entity::update();
	
	// Fade in/out sprite
	if (clock->getElapsedTime() >= alphaDt) {
		clock->restart();
		auto c = sprite->getSprite().getColor();
		if (fading) {
			c.a -= 1;
			if (c.a < 5) {
				fading = false;
				// Reset the position
				position.x = (1 - lif::LEVEL_WIDTH) * lif::TILE_SIZE;
				position.y = (1 - lif::LEVEL_HEIGHT) * lif::TILE_SIZE;
			}
		} else {
			c.a += 1;
			if (c.a > 200)
				fading = true; 
		}
		sprite->getSprite().setColor(c);
	}
}
