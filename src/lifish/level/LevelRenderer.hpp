#pragma once

#include <SFML/Graphics.hpp>
#include "WithOrigin.hpp"

namespace Game {

class LevelManager;

class LevelRenderer final : public sf::Drawable {
	Game::LevelManager& owner;

public:
	explicit LevelRenderer(Game::LevelManager& owner);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
