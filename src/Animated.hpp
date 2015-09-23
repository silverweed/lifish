#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include "AnimatedSprite.hpp"
#include "Entity.hpp"
#include "Clocked.hpp"

namespace Game {

/** It's mandatory that directions are the first 4 elements */
enum : unsigned short {
	ANIM_UP    = 0,
	ANIM_RIGHT = 1,
	ANIM_DOWN  = 2,
	ANIM_LEFT  = 3,
	ANIM_DEATH = 4,
	ANIM_WIN   = 5,
	ANIM_HURT  = 6
};

/**
 * An Animated is a drawable object whose sprite has a certain
 * number of associated animations.
 */
class Animated : public Game::Entity, public Game::Clocked {
protected:
	static constexpr unsigned short MAX_N_ANIMATIONS = 7;
	Animation animations[MAX_N_ANIMATIONS];
	AnimatedSprite animatedSprite;
	sftools::Chronometer frameClock;

public:
	Animated(const sf::Vector2f& pos, const std::string& texture_name = "") 
		: Game::Entity(pos, texture_name) 
		, Game::Clocked(&frameClock)
	{
		animatedSprite.setPosition(pos);
	}
	virtual ~Animated() {}

	virtual void draw(sf::RenderTarget& window) override {
		window.draw(animatedSprite);
	}

	virtual void setPosition(const sf::Vector2f& pos) override {
		Game::Entity::setPosition(pos);
		animatedSprite.setPosition(pos);
	}

	virtual void setOrigin(const sf::Vector2f& origin) override {
		animatedSprite.setOrigin(origin);
	}
};

}
