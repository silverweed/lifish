#pragma once

#include "Pond.hpp"
#include <SFML/Graphics.hpp>
#include <array>

namespace lif {

class Sprite;

class RexFlame : public lif::Pond, public sf::Drawable {
	std::array<lif::Sprite*, 2> sprites;

	sf::Time spriteT,
	         durationT;

	int spriteOffset = 0;

public:
	explicit RexFlame(const sf::Vector2f& pos, const sf::Vector2f& size);

	void update() override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
