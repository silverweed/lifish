#pragma once

#include <array>
#include <SFML/Graphics.hpp>
#include "Pond.hpp"

namespace lif {

class Sprite;
class Clock;

class RexFlame : public lif::Pond, public sf::Drawable {
	std::array<lif::Sprite*, 2> sprites;

	lif::Clock *spriteClock = nullptr,
	           *durationClock = nullptr;

	int spriteOffset = 0;

public:
	explicit RexFlame(const sf::Vector2f& pos, const sf::Vector2f& size);

	void update() override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
