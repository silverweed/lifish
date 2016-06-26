#pragma once

#include <SFML/Graphics.hpp>
#include "WithOrigin.hpp"

namespace Game {

class LevelManager;

class LevelRenderer final : public sf::Drawable, public Game::WithOrigin {
	Game::LevelManager& owner;

public:
	explicit LevelRenderer(Game::LevelManager& owner);

	void setOrigin(const sf::Vector2f& origin) override;
	
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
