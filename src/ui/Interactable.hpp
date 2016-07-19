#pragma once

#include <SFML/Graphics.hpp>
#include "ShadedText.hpp"

namespace Game {

namespace UI {

/** Wrapper class for the glue code needed to merge functionality of
 *  Game::ShadedText and sf::Sprite
 */
class Interactable : public sf::Drawable, public Game::WithOrigin {
	enum class Type {
		TEXT, SPRITE
	};
	union {
		Game::ShadedText *text;
		sf::Sprite *sprite;
	};
	const Type type;

public:
	explicit Interactable(Game::ShadedText *text) 
		: text(text)
		, type(Type::TEXT)
	{}
	explicit Interactable(sf::Sprite *sprite)
		: sprite(sprite)
		, type(Type::SPRITE)
	{}
	~Interactable() {
		switch (type) {
		case Type::TEXT: delete text; return;
		case Type::SPRITE: delete sprite; return;
		}
	}

	void setPosition(const sf::Vector2f& pos) {
		switch (type) {
		case Type::TEXT: text->setPosition(pos); return;
		case Type::SPRITE: sprite->setPosition(pos); return;
		}
	}

	void setColor(sf::Color color) {
		switch (type) {
		case Type::TEXT: text->setFGColor(color); return;
		case Type::SPRITE: sprite->setColor(color); return;
		}
	}

	sf::Vector2f getPosition() const {
		switch (type) {
		case Type::TEXT: return text->getPosition(); 
		case Type::SPRITE: return sprite->getPosition(); 
		}
		return sf::Vector2f(0, 0);
	}

	sf::FloatRect getGlobalBounds() const {
		switch (type) {
		case Type::TEXT: return text->getGlobalBounds(); 
		case Type::SPRITE: return sprite->getGlobalBounds(); 
		}
		return sf::FloatRect(0, 0, 0, 0);
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		switch (type) {
		case Type::TEXT: target.draw(*text, states); return;
		case Type::SPRITE: target.draw(*sprite, states);  return;
		}
	}

	void setOrigin(const sf::Vector2f& pos) override {
		switch (type) {
		case Type::TEXT: text->setOrigin(pos); return;
		case Type::SPRITE: sprite->setOrigin(pos); return;
		}
	}
};

}

}
