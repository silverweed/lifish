#pragma once

#include <exception>
#include <variant>
#include <SFML/Graphics.hpp>
#include "ShadedText.hpp"

namespace lif {

namespace ui {

/** Wrapper class for the glue code needed to merge functionality of
 *  lif::ShadedText and sf::Sprite
 */
class Interactable : public sf::Drawable, public lif::WithOrigin {
	std::variant<std::unique_ptr<lif::ShadedText>, std::unique_ptr<sf::Sprite>> inner;

public:
	template <typename T>
	explicit Interactable(T* x)
		: inner(std::unique_ptr<T>(x))
	{}

	void setPosition(const sf::Vector2f& pos) {
		if (auto *text = std::get_if<std::unique_ptr<lif::ShadedText>>(&inner)) 
			(*text)->setPosition(pos);
		else 
			std::get<std::unique_ptr<sf::Sprite>>(inner)->setPosition(pos);
	}

	void setColor(sf::Color color) {
		if (auto *text = std::get_if<std::unique_ptr<lif::ShadedText>>(&inner)) 
			(*text)->setFGColor(color);
		else 
			std::get<std::unique_ptr<sf::Sprite>>(inner)->setColor(color);
	}

	sf::Vector2f getPosition() const {
		if (auto *text = std::get_if<std::unique_ptr<lif::ShadedText>>(&inner)) 
			return (*text)->getPosition();
		else 
			return std::get<std::unique_ptr<sf::Sprite>>(inner)->getPosition();
	}

	sf::FloatRect getGlobalBounds() const {
		if (auto *text = std::get_if<std::unique_ptr<lif::ShadedText>>(&inner)) 
			return (*text)->getGlobalBounds();
		else 
			return std::get<std::unique_ptr<sf::Sprite>>(inner)->getGlobalBounds();
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		if (auto *text = std::get_if<std::unique_ptr<lif::ShadedText>>(&inner)) 
			target.draw(**text, states);
		else 
			target.draw(*std::get<std::unique_ptr<sf::Sprite>>(inner), states);
	}

	void setOrigin(const sf::Vector2f& pos) override {
		if (auto *text = std::get_if<std::unique_ptr<lif::ShadedText>>(&inner)) 
			(*text)->setOrigin(pos);
		else 
			std::get<std::unique_ptr<sf::Sprite>>(inner)->setOrigin(pos);
	}

	lif::ShadedText* getText() const {
		return std::get<std::unique_ptr<lif::ShadedText>>(inner).get();
	}

	sf::Sprite* getSprite() const {
		return std::get<std::unique_ptr<sf::Sprite>>(inner).get();
	}

	void setScale(float x, float y) {
		if (auto *text = std::get_if<std::unique_ptr<lif::ShadedText>>(&inner)) 
			(*text)->setScale(x, y);
		else 
			std::get<std::unique_ptr<sf::Sprite>>(inner)->setScale(x, y);
	}
};

}

}
