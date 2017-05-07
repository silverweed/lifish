#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "ShadedText.hpp"

namespace lif {

class FPSDisplayer : public sf::Drawable {

	lif::ShadedText fpsText;
	sf::Clock fpsClock, fpsUpdateClock;
	double curTime = 0;
	int nUpdates = 0;

public:
	explicit FPSDisplayer(const sf::Vector2f& pos, const std::string& fontname);

	void update();

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

}
