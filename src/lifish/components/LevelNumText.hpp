#pragma once

#include "Component.hpp"
#include "ShadedText.hpp"
#include <SFML/Graphics.hpp>

namespace lif {

class LevelNumText : public lif::Component, public sf::Drawable {
	sf::RectangleShape bg;
	lif::ShadedText text;

public:
	explicit LevelNumText(lif::Entity& owner, unsigned short levelnum);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void setOrigin(const sf::Vector2f& pos) override;
};

}
