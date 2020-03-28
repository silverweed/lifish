#pragma once

#include <SFML/Graphics.hpp>
#include "ShadedText.hpp"

namespace lif {

class GameTimer : public sf::Drawable {
public:
	explicit GameTimer(const std::string& fontname);
	void update();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	sf::Time time;
	lif::ShadedText text;
	sf::RectangleShape bg;
};
}
