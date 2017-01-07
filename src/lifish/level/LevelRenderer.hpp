#pragma once

#include <SFML/Graphics.hpp>

namespace lif {

class LevelManager;

class LevelRenderer final : public sf::Drawable {
	lif::LevelManager& owner;

public:
	explicit LevelRenderer(lif::LevelManager& owner);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
