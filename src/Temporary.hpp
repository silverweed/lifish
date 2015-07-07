#pragma once

#include "FixedEntity.hpp"
#include "Animated.hpp"

namespace Game {

class Temporary : public Game::FixedEntity, public Game::Animated {
public:
	Temporary(const sf::Vector2f& pos, const std::string& texture_name)
		: FixedEntity(pos, texture_name) {}
	virtual void play() { animatedSprite.play(); }
	virtual bool isPlaying() const { return animatedSprite.isPlaying(); }

	void setOrigin(const sf::Vector2f& pos) override { Game::Animated::setOrigin(pos); }
	void draw(sf::RenderTarget& window) override {
		animatedSprite.update(frameClock.restart());
		Game::Animated::draw(window);
	}
};

}
