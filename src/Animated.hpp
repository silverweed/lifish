#pragma once
/**
 * An Animated is a drawable object whose sprite has a certain
 * number of associated animations.
 */

#include <string>
#include <SFML/Graphics.hpp>
#include "AnimatedSprite.hpp"
#include "Drawable.hpp"

namespace Game {

class Animated : public Game::Drawable {
protected:
	static constexpr unsigned short MAX_N_ANIMATIONS = 7;
	Animation animations[MAX_N_ANIMATIONS];
	AnimatedSprite animatedSprite;
	sf::Clock frameClock;
public:
	virtual void draw(sf::RenderTarget& window) override;
	virtual void setOrigin(const sf::Vector2f& _origin) {
		animatedSprite.setOrigin(_origin);
	}
	virtual void setPosition(const sf::Vector2f& _pos) {
		animatedSprite.setPosition(_pos);
	}
};

}
