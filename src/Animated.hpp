#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include "AnimatedSprite.hpp"
#include "Drawable.hpp"

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
class Animated : public Game::Drawable {
protected:
	static constexpr unsigned short MAX_N_ANIMATIONS = 7;
	Animation animations[MAX_N_ANIMATIONS];
	AnimatedSprite animatedSprite;
	sf::Clock frameClock;
public:
	virtual ~Animated() {}

	virtual void draw(sf::RenderTarget& window) override;
	virtual void setOrigin(const sf::Vector2f& _origin) {
		animatedSprite.setOrigin(_origin);
	}
	virtual void setPosition(const sf::Vector2f& _pos) {
		animatedSprite.setPosition(_pos);
	}

	void resetFrameClock() {
		frameClock.restart();
	}
};

}
