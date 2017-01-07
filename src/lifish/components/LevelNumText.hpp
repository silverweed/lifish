#pragma once

#include <SFML/Graphics.hpp>
#include "Component.hpp"
#include "ShadedText.hpp"

namespace lif {

class LevelNumText : public lif::Component, public sf::Drawable {
	lif::ShadedText text;

public:
	explicit LevelNumText(lif::Entity& owner, 
			const unsigned short levelnum);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void setOrigin(const sf::Vector2f& pos) override;
};

}
