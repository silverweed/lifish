#pragma once

#include <SFML/Graphics.hpp>
#include "Sprite.hpp"
#include "ShadedText.hpp"

namespace Game {

class Clickable : public sf::Drawable {
	union {
		Game::ShadedText *text;
		Game::Sprite *sprite;
	};
	sf::Drawable *drawable = nullptr;

	const bool isText;

public:
	explicit Clickable(Game::ShadedText *e) 
		: text(e)
		, drawable(e)
		, isText(true) {}

	explicit Clickable(Game::Sprite *e) 
		: sprite(e)
		, drawable(e)
		, isText(false) {}

	~Clickable() {
		if (isText && text != nullptr) delete text;
		else if (sprite != nullptr) delete sprite;
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(*drawable, states);
	}

	bool isBelow(const sf::Vector2f& pos) const {
		if (isText)
			return text->getGlobalBounds().contains(pos.x, pos.y);
		else
			return sprite->getSprite().getGlobalBounds().contains(pos.x, pos.y);
	}

	void highlight(bool on) {
		if (isText)
			text->setFGColor(on ? sf::Color::Red : sf::Color::White);
		else
			sprite->getSprite().setColor(on ? sf::Color::Red : sf::Color::White);
	}
};

}
