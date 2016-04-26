#pragma once

#include <SFML/Graphics.hpp>

namespace Game {

class LevelManager;

class LevelRenderer final : public sf::Drawable, public Game::WithOrigin {
	const Game::LevelManager *owner;

public:
	explicit LevelRenderer(const Game::LevelManager *owner);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
