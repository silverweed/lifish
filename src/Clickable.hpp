#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "Sprite.hpp"
#include "ShadedText.hpp"

namespace Game {

class Clickable : public Game::WithOrigin, public sf::Drawable {
	std::unique_ptr<sf::Drawable> drawable;
	std::function<bool(const sf::Vector2f&)> isBelowCb;
	std::function<void(bool)> highlightCb;

public:
	explicit Clickable(sf::Drawable *e, std::function<bool(const sf::Vector2f&)> isBelow,
			std::function<void(bool)> highlight)
		: Game::WithOrigin()
		, drawable(e)
		, isBelowCb(isBelow)
		, highlightCb(highlight) {}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		target.draw(*drawable.get(), states);
	}

	bool isBelow(const sf::Vector2f& pos) const {
		return isBelowCb(pos);
	}

	void highlight(bool on) {
		highlightCb(on);
	}

	void setOrigin(const sf::Vector2f& origin) override {
		auto o = dynamic_cast<Game::WithOrigin*>(drawable.get());
		if (o != nullptr)
			o->setOrigin(origin);
	}
};

}
