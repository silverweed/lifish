#pragma once

#include "Animated.hpp"

namespace Game {

/**
 * A Temporary is an animated entity which gets deleted by the
 * Game::LevelRenderer as soon as isPlaying() == false.
 */
class Temporary : public Game::Animated {
public:
	Temporary(const sf::Vector2f& pos)
		: Game::Animated(pos) {}
	Temporary(const sf::Vector2f& pos, const std::string& texture_name)
		: Game::Animated(pos, texture_name) {}

	virtual void play() { animatedSprite.play(); }
	virtual bool isPlaying() const { return animatedSprite.isPlaying(); }

	virtual void draw(sf::RenderTarget& window) override {
		animatedSprite.update(frameClock.restart());
		Game::Animated::draw(window);
	}
};

}
