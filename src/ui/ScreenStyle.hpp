#pragma once

#include <SFML/Graphics.hpp>

namespace lif {

namespace ui {

struct ScreenStyle {
	std::string name;

	int spacing = 0;
	bool interactable = false;
	std::string font;
	unsigned int characterSize = 0;
	std::string vAlign, hAlign;
	sf::Color color = sf::Color::White;
	sf::Color bgcolor = sf::Color::Black;
	int shadowSpacing = 2;
};

}

}
