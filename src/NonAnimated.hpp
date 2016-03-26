#pragma once

#include "Entity.hpp"

namespace Game {

class NonAnimated : public Game::Entity {
protected:
	sf::Sprite sprite;

public:
	NonAnimated(const sf::Vector2f& pos, const std::string& texture_name = "") 
		: Game::Entity(pos, texture_name)
	{
		sprite.setTexture(*texture);
		sprite.setPosition(pos);
	}

	virtual void draw(sf::RenderTarget& window) override {
		window.draw(sprite);
	}

	virtual sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }

	virtual void setPosition(const sf::Vector2f& pos) override {
		Game::Entity::setPosition(pos);
		sprite.setPosition(pos);
	}

	virtual void setOrigin(const sf::Vector2f& origin) override {
		sprite.setOrigin(origin);
	}
};

}
