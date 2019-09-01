#pragma once

#include "ShadedText.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace lif {

class FPSDisplayer : public sf::Drawable {

	lif::ShadedText fpsText;
	sf::Clock fpsClock, fpsUpdateClock;
	double curTime = 0;
	int nUpdates = 0;

public:
	explicit FPSDisplayer(const sf::Vector2f& pos, const std::string& fontname);

	void update(const sf::Vector2u& windowBounds);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

}
