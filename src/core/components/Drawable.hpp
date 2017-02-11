#pragma once

#include <SFML/Graphics.hpp>
#include "Component.hpp"

namespace lif {

class Drawable : public lif::Component, public sf::Drawable {
	const sf::Drawable& delegate;

public:
	explicit Drawable(lif::Entity& owner, const sf::Drawable& delegate)
		: lif::Component(owner)
		, delegate(delegate) 
	{
		keys.emplace_back(_getKey<Drawable>());
	}

	void draw(sf::RenderTarget& window, sf::RenderStates states) const override {
		if (active)
			window.draw(delegate, states);
	}
};

}
