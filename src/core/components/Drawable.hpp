#pragma once

#include <SFML/Graphics.hpp>
#include "Component.hpp"

namespace Game {

class Drawable : public Game::Component, public sf::Drawable {
	const sf::Drawable& delegate;

public:
	explicit Drawable(Game::Entity& owner, const sf::Drawable& delegate)
		: Game::Component(owner)
		, delegate(delegate) {}

	void draw(sf::RenderTarget& window, sf::RenderStates states) const override {
		if (active)
			window.draw(delegate, states);
	}
};

}
